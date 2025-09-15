#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "text_chat.hpp"
#include "lobby.hpp"
#include "mutes.hpp"
#include "ui.hpp"

#include <utils/io.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace text_chat::lobby
{
	namespace
	{
		utils::hook::detour on_lobby_chat_msg_hook;

		void process_chat_msg(game::LobbyChatMsg_t* msg)
		{
			if (!is_chat_enabled())
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

			if (size < 4 || *reinterpret_cast<int*>(buffer) != chat_message_msg_id)
			{
				return;
			}

			if (mutes::is_muted(user.bits))
			{
				return;
			}

			auto text = &buffer[sizeof(chat_message_msg_id)];
			text[chat_message_max_len] = 0;

			const auto name = steam_friends->__vftable->GetFriendPersonaName(steam_friends, user);
			const auto message = utils::string::va("%s: %s", name, text);
			ui::print(message, true);
		}

		int on_lobby_chat_msg_stub(void* a1, game::LobbyChatMsg_t* msg)
		{
			process_chat_msg(msg);
			return on_lobby_chat_msg_hook.invoke<int>(a1, msg);
		}
	}

	void send_chat_message(const std::string& text)
	{
		const auto unk = *game::s_unk1;
		if (unk == nullptr)
		{
			return;
		}

		std::string buffer;
		buffer.append(reinterpret_cast<const char*>(&chat_message_msg_id), sizeof(chat_message_msg_id));
		buffer.append(text);

		const auto steam_matchmaking = (*game::SteamMatchmaking)();
		steam_matchmaking->__vftable->SendLobbyChatMsg(steam_matchmaking, unk->unk1->lobby_id, buffer.data(), static_cast<int>(buffer.size()));
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

			on_lobby_chat_msg_hook.create(0x1405A3DF0, on_lobby_chat_msg_stub);

			command::add("say", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto msg = params.join(1);
				send_chat_message(msg);
			});
		}
	};
}

REGISTER_COMPONENT(text_chat::lobby::component)
