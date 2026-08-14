#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "lobby.hpp"
#include "mutes.hpp"
#include "ui.hpp"
#include "defs.hpp"
#include "../session.hpp"

#include <utils/io.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace text_chat::lobby
{
	namespace
	{
		utils::hook::detour on_lobby_chat_msg_hook;

		constexpr auto max_chat_msg_len = 128;
		constexpr auto chat_message_msg_id_ascii = 20;
		constexpr auto chat_message_msg_id_utf16 = 30;
		constexpr auto chat_team_message_msg_id_ascii = 21;
		constexpr auto chat_team_message_msg_id_utf16 = 31;

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

		void process_chat_msg(game::LobbyChatMsg_t* msg)
		{
			if (!game::environment::is_dedi() && !text_chat::is_chat_enabled())
			{
				return;
			}

			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			const auto steam_friends = (*game::SteamFriends)();

			game::steam_id user{};
			char buffer[4096]{};
			int chat_entry_type{};
			const auto size = steam_matchmaking->__vftable->GetLobbyChatEntry(steam_matchmaking, msg->lobby_id, msg->chat_id,
				&user, buffer, sizeof(buffer), &chat_entry_type);

			const auto match_container = game::s_mgoMatchMakingManager->match_container;
			if (match_container == nullptr || match_container->match->lobby_id.bits != msg->lobby_id.bits)
			{
				return;
			}

			if (size < 5)
			{
				return;
			}

			const auto msg_id = *reinterpret_cast<int*>(buffer);

			if ((msg_id != chat_message_msg_id_ascii && 
				msg_id != chat_team_message_msg_id_ascii &&
				msg_id != chat_message_msg_id_utf16 &&
				msg_id != chat_team_message_msg_id_utf16) ||
				mutes::is_muted(user.bits))
			{
				return;
			}

			auto is_self = false;
			auto index = 0;
			const auto client = session::get_client_by_steam_id(user.bits, &is_self, &index);
			if (client == nullptr)
			{
				return;
			}

			auto name = session::get_player_name(static_cast<unsigned char>(index));
			if (name == nullptr)
			{
				name = steam_friends->__vftable->GetFriendPersonaName(steam_friends, user);
				if (name == nullptr)
				{
					return;
				}
			}

			const auto is_team_message = msg_id == chat_message_msg_id_ascii || msg_id == chat_team_message_msg_id_utf16;

			const auto self_team = session::get_self_team();
			const auto other_team = session::get_player_team(user.bits);

			std::wstring text;

			if (msg_id == chat_message_msg_id_ascii || msg_id == chat_team_message_msg_id_ascii)
			{
				auto text_buffer = &buffer[sizeof(chat_message_msg_id_ascii)];
				text_buffer[max_chat_msg_len * sizeof(char)] = 0;
				text = utils::string::convert(text_buffer);
			}
			else
			{
				auto text_buffer = reinterpret_cast<wchar_t*>(&buffer[sizeof(chat_message_msg_id_ascii)]);
				text_buffer[max_chat_msg_len * sizeof(wchar_t)] = 0;
				text = text_buffer;
			}

			std::wstring formatted_msg;
			formatted_msg.resize(0x400);

			const auto send_msg = [](const std::wstring& message)
			{
				if (!game::environment::is_dedi())
				{
					text_chat::ui::print(message, true);
				}

				const auto message_a = utils::string::utf16_to_ascii(message);
				console::info("%s\n", message_a.data());
			};

			const auto name_w = utils::string::utf8_to_utf16(name);

			if (is_team_message && (other_team == self_team) && other_team != -1 && self_team != -1)
			{
				swprintf_s(formatted_msg.data(), formatted_msg.size(), L"^8(%s) %s^7: ^2%s", get_team_name(other_team), name_w.data(), text.data());
				send_msg(formatted_msg);
			}
			else if ((is_team_message && other_team == -1) || !is_team_message)
			{
				swprintf_s(formatted_msg.data(), formatted_msg.size(), L"^%c%s^7: %s", get_team_color(self_team, other_team), name_w.data(), text.data());
				send_msg(formatted_msg);
			}
		}

		int on_lobby_chat_msg_stub(void* a1, game::LobbyChatMsg_t* msg)
		{
			process_chat_msg(msg);
			return on_lobby_chat_msg_hook.invoke<int>(a1, msg);
		}
	}

	void send_chat_message(const std::wstring& text, bool team)
	{
		const auto match_container = game::s_mgoMatchMakingManager->match_container;
		if (match_container == nullptr)
		{
			return;
		}

		const auto id = team
			? chat_team_message_msg_id_utf16
			: chat_message_msg_id_utf16;

		std::string buffer;
		buffer.append(reinterpret_cast<const char*>(&id), sizeof(id));
		buffer.append(reinterpret_cast<const char*>(text.data()), text.size() * 2);

		const auto steam_matchmaking = (*game::SteamMatchmaking)();
		steam_matchmaking->__vftable->SendLobbyChatMsg(steam_matchmaking, match_container->match->lobby_id, buffer.data(), static_cast<int>(buffer.size()));
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{

		}

		void start() override
		{
			if (!game::environment::is_mgo())
			{
				return;
			}

			on_lobby_chat_msg_hook.create(SELECT_VALUE_LANG(0x1405A4000, 0x0), on_lobby_chat_msg_stub);

			command::add("say", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto msg = utils::string::convert(params.join(1));
				send_chat_message(msg, false);
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
	};
}

REGISTER_COMPONENT(text_chat::lobby::component)
