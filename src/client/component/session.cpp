#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "command.hpp"
#include "scheduler.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace session
{
	namespace
	{
		utils::hook::detour create_lobby_cb_hook;

		game::steam_id current_lobby_id;
		std::unordered_set<std::uint64_t> kicked_steam_ids;

		struct kick_msg_t
		{
			std::uint32_t type;
			std::uint32_t unk;
			std::uint64_t steam_id;
		};

		void print_status()
		{
			const auto main_session = *game::s_pSession;
			if (!main_session)
			{
				return;
			}

			const auto steam_friends = (*game::SteamFriends)();
			const auto is_host = main_session->sessionInterface.__vftable->IsHost(&main_session->sessionInterface);
			const auto state = main_session->__vftable->GetState(main_session);
			const auto all_members = main_session->__vftable->GetAllMembers(main_session);

			printf("is host: %i\n", is_host);
			printf("state: %i\n", state);
			printf("num flags steam_id                         name                            \n");
			printf("--- ----- -------------------------------- --------------------------------\n");

			for (auto i = 0u; i < all_members->size; i++)
			{
				const auto member = all_members->members[i];
				if (member == nullptr || member->flags == 0)
				{
					continue;
				}

				game::steam_id steam_id{};
				steam_id.bits = member->sessionUserId->userId;
				const auto name = steam_friends->__vftable->GetFriendPersonaName(steam_friends, steam_id);

				printf("%3i %5i %32lli %32s", i, member->flags, steam_id.bits, name);
			}
		}

		void set_lobby_data(const std::string& key, const std::string& value)
		{
			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			const auto res = steam_matchmaking->__vftable->SetLobbyData(steam_matchmaking, current_lobby_id, key.data(), value.data());
			printf("[SteamMatchmaking] SetLobbyData(%s, %s) = %i\n", key.data(), value.data(), res);
		}

		void set_lobby_data(const std::string& key, const std::uint64_t value)
		{
			set_lobby_data(key, utils::string::va("%llu", value));
		}

		const char* get_lobby_data(const std::string& key)
		{
			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			return steam_matchmaking->__vftable->GetLobbyData(steam_matchmaking, current_lobby_id, key.data());
		}

		void ban_player_from_lobby(const std::uint64_t steam_id)
		{
			kicked_steam_ids.insert(steam_id);

			set_lobby_data("kick_num", kicked_steam_ids.size());

			auto index = 0;
			for (const auto& id : kicked_steam_ids)
			{
				set_lobby_data(utils::string::va("kicked_id_%d", index++), id);
			}
		}

		void kick_player_from_lobby(const std::uint64_t steam_id)
		{
			kick_msg_t kick_msg{};
			kick_msg.type = 1;
			kick_msg.unk = 0xFFFFFFFF;
			kick_msg.steam_id = steam_id;

			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			steam_matchmaking->__vftable->SendLobbyChatMsg(steam_matchmaking, current_lobby_id, &kick_msg, sizeof(kick_msg));
		}

		bool is_host()
		{
			const auto main_session = *game::s_pSession;
			if (main_session == nullptr)
			{
				return false;
			}

			return main_session->sessionInterface.__vftable->IsHost(&main_session->sessionInterface);
		}

		game::fox::nt::Member* get_client_by_index(const unsigned int index)
		{
			const auto main_session = *game::s_pSession;
			if (main_session == nullptr || index >= main_session->allMembers.size)
			{
				return nullptr;
			}

			const auto local_member = main_session->__vftable->GetLocalMember(main_session);
			const auto target_member = main_session->allMembers.members[index];

			if (target_member == nullptr || target_member->flags || (local_member->sessionUserId->userId == target_member->sessionUserId->userId))
			{
				return nullptr;
			}

			return target_member;
		}

		void* create_lobby_cb_stub(void* a1, game::steam_id lobby_id)
		{
			current_lobby_id = lobby_id;
			printf("[SteamMatchmaking] Created lobby %llu\n", lobby_id.bits);
			return create_lobby_cb_hook.invoke<void*>(a1, lobby_id);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_mgo())
			{
				return;
			}

			command::add("status", []
			{
				scheduler::once(print_status, scheduler::session);
			});

			command::add("kick", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					printf("usage: kick <player index>\n");
					return;
				}

				const auto index_s = params.get(1);
				const auto index = std::atoi(index_s.data());
				scheduler::once([index]
				{
					if (!is_host())
					{
						printf("cannot kick as non-host\n");
						return;
					}

					const auto client = get_client_by_index(index);
					if (client == nullptr)
					{
						printf("client not found\n");
						return;
					}

					kick_player_from_lobby(client->sessionUserId->userId);
					ban_player_from_lobby(client->sessionUserId->userId);
					game::fox::nt::Member_::Reset(client);
				}, scheduler::session);
			});

			command::add("clearkicks", []()
			{
				scheduler::once([]
				{
					kicked_steam_ids.clear();
					set_lobby_data("kick_num", 0);
				}, scheduler::session);
			});

			create_lobby_cb_hook.create(0x144EF10B0, create_lobby_cb_stub);
		}
	};
}

REGISTER_COMPONENT(session::component)
