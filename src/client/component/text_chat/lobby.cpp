#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "lobby.hpp"
#include "mutes.hpp"
#include "ui.hpp"
#include "defs.hpp"
#include "../session.hpp"
#include "../scripting.hpp"
#include "../matchmaking.hpp"
#include "../game_socket.hpp"

#include <utils/io.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace text_chat::lobby
{
	namespace
	{
		utils::hook::detour on_lobby_chat_msg_hook;

		vars::var_ptr var_scr_chat_callback;

		constexpr auto chat_message_msg_id = 30;
		constexpr auto chat_team_message_msg_id = 31;

		using process_msg_fn_t = void (__fastcall*)(game::fox::nt::Member*, const char*, const std::size_t);
		process_msg_fn_t process_chat_msg_fn = nullptr;

		const wchar_t* get_team_name(char team)
		{
			switch (team)
			{
			case 0:
				return L"Solid";
			case 1:
				return L"Liquid";
			}

			return L"";
		}

		char get_team_color(char self, char other)
		{
			if (other == -1 || self == -1)
			{
				return '7';
			}

			return other == self ? '8' : '9';
		}

		void script_chat_notify(const int player_index, const std::string& message)
		{
			const auto ruleset = session::get_active_ruleset();
			if (ruleset == nullptr || player_index >= 16)
			{
				return;
			}

			const auto lock = scripting::acquire_lock();
			static auto func_name = game::fox::GetBinaryStringHandle("OnPlayerSay");
			auto a2 = 0;

			const auto ruleset_data = ruleset->unk2 != nullptr ? ruleset->unk2->rulesetData : nullptr;
			const auto player = ruleset->players[player_index];

			game::fox::LuaPushEntity(lock->get_lua_state(), ruleset_data);
			game::fox::LuaPushEntity(lock->get_lua_state(), ruleset);
			game::fox::LuaPushEntity(lock->get_lua_state(), player);
			game::lua::lua_pushstring(lock->get_lua_state(), message.data());

			game::fox::Script_::CallScriptFunc(ruleset->script, &a2, &func_name, lock->get_lua(), 4, 0);
		}

		std::wstring get_steam_player_name(game::steam_id user)
		{
			const auto steam_friends = (*game::SteamFriends)();
			const auto name = steam_friends->__vftable->GetFriendPersonaName(steam_friends, user);
			const auto name_w = utils::string::utf8_to_utf16(name);
			return name_w;
		}

		void process_chat_msg_tpp(game::fox::nt::Member* client, const char* buffer, const std::size_t size)
		{
			if (size < 6)
			{
				return;
			}

			game::steam_id user{};
			user.bits = client->sessionUserId->userId;

			const auto msg_id = *reinterpret_cast<const int*>(buffer);
			if (msg_id != chat_message_msg_id)
			{
				return;
			}

			const auto name_w = get_steam_player_name(user);
			const auto text = reinterpret_cast<const wchar_t*>(&buffer[sizeof(chat_message_msg_id)]);
			auto text_len = (size - sizeof(chat_message_msg_id)) / sizeof(wchar_t);

			if (text_len > chat_message_max_len)
			{
				text_len = chat_message_max_len;
			}

			std::wstring formatted_msg;
			formatted_msg.resize(chat_message_buffer_len);

			const auto self = matchmaking::get_current_steam_id();
			if (user.bits == self.bits)
			{
				_snwprintf_s(formatted_msg.data(), formatted_msg.size(), _TRUNCATE,
					L"^7%s^7: %.*s", name_w.data(), static_cast<int>(text_len), text);
			}
			else
			{
				_snwprintf_s(formatted_msg.data(), formatted_msg.size(), _TRUNCATE,
					L"^1%s^7: %.*s", name_w.data(), static_cast<int>(text_len), text);
			}

			text_chat::ui::print(formatted_msg, true);

			const auto message_a = utils::string::utf16_to_ascii(formatted_msg);
			console::info("%s\n", message_a.data());
		}

		void process_chat_msg_mgo(game::fox::nt::Member* client, const char* buffer, const std::size_t size)
		{
			if (size < 6)
			{
				return;
			}

			game::steam_id user{};
			user.bits = client->sessionUserId->userId;

			const auto msg_id = *reinterpret_cast<const int*>(buffer);
			if ((msg_id != chat_message_msg_id && msg_id != chat_team_message_msg_id) || 
				mutes::is_muted(user.bits))
			{
				return;
			}

			const auto steam_friends = (*game::SteamFriends)();
			const auto index = client->sessionInterface.__vftable->GetIndex(&client->sessionInterface);

			auto name = session::get_player_name(static_cast<unsigned char>(index));
			if (name == nullptr || name[0] == 0)
			{
				name = steam_friends->__vftable->GetFriendPersonaName(steam_friends, user);
				if (name == nullptr)
				{
					return;
				}
			}

			const auto name_w = utils::string::utf8_to_utf16(name);
			const auto is_team_message = msg_id == chat_team_message_msg_id;
			const auto self_team = session::get_self_team();
			const auto other_team = session::get_player_team(user.bits);

			const auto text = reinterpret_cast<const wchar_t*>(&buffer[sizeof(chat_message_msg_id)]);
			auto text_len = (size - sizeof(chat_message_msg_id)) / sizeof(wchar_t);
			if (text_len > chat_message_max_len)
			{
				text_len = chat_message_max_len;
			}

			std::wstring formatted_msg;
			formatted_msg.resize(chat_message_buffer_len);

			if (is_team_message && (other_team == self_team) && other_team != -1 && self_team != -1)
			{
				_snwprintf_s(formatted_msg.data(), formatted_msg.size(), _TRUNCATE, 
					L"^8(%s) %s^7: ^2%.*s", get_team_name(other_team), name_w.data(), static_cast<int>(text_len), text);
			}
			else if ((is_team_message && other_team == -1) || !is_team_message)
			{
				_snwprintf_s(formatted_msg.data(), formatted_msg.size(), _TRUNCATE,
					L"^%c%s^7: %.*s", get_team_color(self_team, other_team), name_w.data(), static_cast<int>(text_len), text);
			}

			if (formatted_msg[0] == 0)
			{
				return;
			}

			if (!game::environment::is_dedi())
			{
				ui::print(formatted_msg, true);
			}

			const auto message_a = utils::string::utf16_to_ascii(formatted_msg);
			console::info("%s\n", message_a.data());

			if (matchmaking::is_host() && var_scr_chat_callback->current.enabled())
			{
				const auto utf8_text = utils::string::utf16_to_utf8(text);
				script_chat_notify(index, utf8_text);
			}
		}

		void process_chat_msg(game::LobbyChatMsg_t* msg)
		{
			if (!game::environment::is_dedi() && !text_chat::is_chat_enabled())
			{
				return;
			}

			const auto steam_matchmaking = (*game::SteamMatchmaking)();

			game::steam_id user{};
			char buffer[0x500]{};
			int chat_entry_type{};
			const auto size = steam_matchmaking->__vftable->GetLobbyChatEntry(steam_matchmaking, msg->lobby_id, msg->chat_id,
				&user, buffer, sizeof(buffer), &chat_entry_type);

			const auto match = matchmaking::get_match();
			if (match->lobby_id.bits != msg->lobby_id.bits)
			{
				return;
			}

			const auto client = session::get_client_by_steam_id(user.bits);
			if (client == nullptr)
			{
				return;
			}

			process_chat_msg_fn(client, buffer, size);
		}

		int on_lobby_chat_msg_stub(void* a1, game::LobbyChatMsg_t* msg)
		{
			process_chat_msg(msg);
			return on_lobby_chat_msg_hook.invoke<int>(a1, msg);
		}

		template <bool Join>
		void member_event_handler(game::fox::nt::impl::SessionImpl2* session, std::uint8_t* arg)
		{
			const auto self = session->sessionInterface.__vftable->GetLocalMemberInterface(&session->sessionInterface);
			const auto index = self->__vftable->GetIndex(self);
			if (arg == nullptr || *arg == static_cast<std::uint8_t>(index))
			{
				return;
			}

			const auto member_interface = session->sessionInterface.__vftable->GetMemberInterfaceAtIndex(&session->sessionInterface, *arg);
			const auto member_index = member_interface->__vftable->GetIndex(member_interface);
			const auto member = session->allMembers.members[member_index];

			if (member == nullptr)
			{
				return;
			}

			game::steam_id user{};
			user.bits = member->sessionUserId->userId;
			const auto name_w = get_steam_player_name(user);

			if constexpr (Join)
			{
				const auto msg = std::format(L"^3{} connected", name_w);
				ui::print(msg, false);
			}
			else
			{
				const auto msg = std::format(L"^3{} disconnected", name_w);
				ui::print(msg, false);
			}
		}
	}

	void send_chat_message(const std::wstring& text, bool team)
	{
		const auto id = team
			? chat_team_message_msg_id
			: chat_message_msg_id;

		std::string buffer;
		buffer.reserve(sizeof(id) + text.size() * sizeof(wchar_t));

		buffer.append(reinterpret_cast<const char*>(&id), sizeof(id));
		buffer.append(reinterpret_cast<const char*>(text.data()), text.size() * sizeof(wchar_t));

		game_socket::send("chat_message", buffer, -1);
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			if (!game::environment::is_mgo())
			{
				return;
			}

			var_scr_chat_callback = vars::register_bool("scr_chat_callback", false, 0, "enable OnPlayerSay ruleset callback");
		}

		void start() override
		{
			if (game::environment::is_mgo())
			{
				on_lobby_chat_msg_hook.create(SELECT_VALUE_LANG(0x1405A4000, 0x0), on_lobby_chat_msg_stub);
				process_chat_msg_fn = process_chat_msg_mgo;
			}
			else
			{
				process_chat_msg_fn = process_chat_msg_tpp;
			}

			game_socket::on("chat_message", [](game::fox::nt::impl::SessionImpl2* session,
				const game_socket::message_info_t& info, const std::string_view& buffer)
			{
				if (info.game_object_id >= 16)
				{
					return;
				}

				const auto sender = session->allMembers.members[info.game_object_id];
				if (sender == nullptr)
				{
					return;
				}

				process_chat_msg_fn(sender, buffer.data(), buffer.size());
			});

			command::add("say", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto msg = utils::string::convert(params.join(1));
				send_chat_message(msg, false);
			});

			if (game::environment::is_mgo())
			{
				game_socket::on("ui_print", [](game::fox::nt::impl::SessionImpl2* session,
					const game_socket::message_info_t& info, const std::string_view& buffer)
				{
					if (game::environment::is_dedi() || info.peer_type != 5 || info.sender != 0)
					{
						return;
					}

					const auto len = std::min(chat_message_max_len, buffer.size() / sizeof(wchar_t));
					std::wstring text{reinterpret_cast<const wchar_t*>(buffer.data()), len};
					const auto ascii = utils::string::utf16_to_ascii(text);

					ui::print(text, false);
					console::info("%s\n", ascii.data());
				});

				command::add("tell", [](const command::params& params)
				{
					if (params.size() < 3)
					{
						return;
					}

					const auto client = params.get_int(1);
					const auto msg = utils::string::utf8_to_utf16(params.join(2));
					const auto buffer_view = std::string_view{reinterpret_cast<const char*>(msg.data()), msg.size() * sizeof(wchar_t)};

					game_socket::send("ui_print", buffer_view, static_cast<std::int8_t>(client), 5);
				});

				command::add("say_team", [](const command::params& params)
				{
					if (params.size() < 2)
					{
						return;
					}

					const auto msg = utils::string::convert(params.join(1));
					send_chat_message(msg, true);
				});
			}
			else
			{
				game_socket::on_session_notify(game::fox::nt::NOTIFY_JOIN_MEMBER, member_event_handler<true>);
				game_socket::on_session_notify(game::fox::nt::NOTIFY_DELETE_MEMBER, member_event_handler<false>);
			}
		}
	};
}

REGISTER_COMPONENT(text_chat::lobby::component)
