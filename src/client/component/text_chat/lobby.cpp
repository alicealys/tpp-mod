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
		constexpr auto chat_message_msg_id = 20;
		constexpr auto chat_team_message_msg_id = 21;

		const char* get_team_name(char team)
		{
			switch (team)
			{
			case 0:
				return "Solid";
			case 1:
				return "Liquid";
			}

			return "";
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
			if (!text_chat::is_chat_enabled())
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

			if ((msg_id != chat_message_msg_id && msg_id != chat_team_message_msg_id) || 
				mutes::is_muted(user.bits))
			{
				return;
			}

			const auto name = steam_friends->__vftable->GetFriendPersonaName(steam_friends, user);
			const auto is_team_message = msg_id == chat_team_message_msg_id;

			const auto self_team = session::get_self_team();
			const auto other_team = session::get_player_team(user.bits);

			auto text = &buffer[sizeof(chat_message_msg_id)];
			text[max_chat_msg_len] = 0;

			const char* message = nullptr;

			if (is_team_message && (other_team == self_team) && other_team != -1 && self_team != -1)
			{
				message = utils::string::va("^8(%s) %s^7: ^2%s", get_team_name(other_team), name, text);
			}
			else if ((is_team_message && other_team == -1) || !is_team_message)
			{
				message = utils::string::va("^%c%s^7: %s", get_team_color(self_team, other_team), name, text);
			}

			if (message != nullptr)
			{
				text_chat::ui::print(message, true);
				console::info("%s\n", message);
			}
		}

		int on_lobby_chat_msg_stub(void* a1, game::LobbyChatMsg_t* msg)
		{
			process_chat_msg(msg);
			return on_lobby_chat_msg_hook.invoke<int>(a1, msg);
		}
	}

	void send_chat_message(const std::string& text, bool team)
	{
		const auto match_container = game::s_mgoMatchMakingManager->match_container;
		if (match_container == nullptr)
		{
			return;
		}

		const auto id = team
			? chat_team_message_msg_id
			: chat_message_msg_id;

		std::string buffer;
		buffer.append(reinterpret_cast<const char*>(&id), sizeof(id));
		buffer.append(text);

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

			on_lobby_chat_msg_hook.create(SELECT_VALUE_LANG(0x1405A3DF0, 0x1405A3810), on_lobby_chat_msg_stub);

			command::add("say", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto msg = params.join(1);
				send_chat_message(msg, false);
			});

			command::add("say_team", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto msg = params.join(1);
				send_chat_message(msg, true);
			});
		}
	};
}

REGISTER_COMPONENT(text_chat::lobby::component)
