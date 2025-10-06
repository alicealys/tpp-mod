#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "scheduler.hpp"
#include "console.hpp"
#include "session.hpp"
#include "dedicated_server.hpp"
#include "matchmaking.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace session
{
	namespace
	{
		const char* get_session_state_name(const int state)
		{
			switch (state)
			{
			case 0:
				return "none";
			case 2:
			case 3:
				return "host";
			case 4:
				return "client connect";
			case 5:
				return "client init";
			case 6:
			case 7:
				return "client established";
			}

			return "unknown";
		}

		int get_rtt(game::fox::nt::impl::SessionImpl2* session)
		{
			if (session->sppSocket == nullptr)
			{
				return 0;
			}

			if (game::environment::is_tpp())
			{
				return session->sppSocket->tpp.rtt_time;
			}
			else
			{
				return session->sppSocket->mgo.rtt_time;
			}
		}

		void print_status()
		{
			const auto main_session = *game::s_pSession;
			if (!main_session)
			{
				return;
			}

			const auto steam_friends = (*game::SteamFriends)();
			const auto is_host = main_session->sessionInterface.__vftable->IsHost(&main_session->sessionInterface);
			const auto state = main_session->__vftable->tpp.GetState(main_session);
			const auto all_members = &main_session->allMembers;

			printf("is host: %i\n", is_host);
			printf("state: %s (%i)\n", get_session_state_name(state), state);
			printf("rtt: %ims\n", get_rtt(main_session));
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

		bool is_host()
		{
			const auto main_session = *game::s_pSession;
			if (main_session == nullptr)
			{
				return false;
			}

			return main_session->sessionInterface.__vftable->IsHost(&main_session->sessionInterface);
		}

		game::fox::nt::Member* get_client_by_index(const unsigned int index, bool* is_self)
		{
			const auto main_session = *game::s_pSession;
			if (main_session == nullptr || index >= main_session->allMembers.size)
			{
				return nullptr;
			}

			const auto local_member = main_session->__vftable->mgo.GetLocalMember(main_session);
			const auto target_member = main_session->allMembers.members[index];

			if (target_member == nullptr || target_member->flags == 0)
			{
				return nullptr;
			}

			*is_self = (local_member->sessionUserId->userId == target_member->sessionUserId->userId);

			return target_member;
		}

		game::fox::nt::Member* get_client_by_steam_id(const std::uint64_t steam_id, bool* is_self)
		{
			const auto main_session = *game::s_pSession;
			if (main_session == nullptr)
			{
				return nullptr;
			}

			const auto local_member = main_session->__vftable->mgo.GetLocalMember(main_session);

			for (auto i = 0u; i < main_session->allMembers.size; i++)
			{
				const auto member = main_session->allMembers.members[i];
				if (member == nullptr || member->flags == 0)
				{
					continue;
				}

				if (member->sessionUserId->userId == steam_id)
				{
					*is_self = local_member->sessionUserId->userId == member->sessionUserId->userId;
					return member;
				}
			}

			return nullptr;
		}

		game::fox::nt::Member* get_client_by_name(const std::string& name, bool* is_self)
		{
			const auto main_session = *game::s_pSession;
			if (main_session == nullptr)
			{
				return nullptr;
			}

			const auto lower = utils::string::to_lower(name);

			const auto local_member = main_session->__vftable->mgo.GetLocalMember(main_session);
			const auto steam_friends = (*game::SteamFriends)();

			for (auto i = 0u; i < main_session->allMembers.size; i++)
			{
				const auto member = main_session->allMembers.members[i];
				if (member == nullptr || member->flags == 0)
				{
					continue;
				}

				game::steam_id steam_id{};
				steam_id.bits = member->sessionUserId->userId;

				const std::string member_name = steam_friends->__vftable->GetFriendPersonaName(steam_friends, steam_id);
				const auto member_name_lower = utils::string::to_lower(name);

				if (member_name_lower.starts_with(lower))
				{
					*is_self = local_member->sessionUserId->userId == member->sessionUserId->userId;
					return member;
				}
			}

			return nullptr;
		}

		game::fox::nt::Member* find_client(const std::string& identifier, bool* is_self)
		{
			const auto is_number = utils::string::is_numeric(identifier);

			if (is_number)
			{
				const auto integer = std::strtoull(identifier.data(), nullptr, 0);
				if (integer < 128)
				{
					return get_client_by_index(static_cast<unsigned int>(integer), is_self);
				}
				else
				{
					return get_client_by_steam_id(integer, is_self);
				}
			}
			else
			{
				return get_client_by_name(identifier, is_self);
			}
		}

		void print_rtt()
		{
			const auto main_session = *game::s_pSession;
			if (!main_session)
			{
				return;
			}

			printf("rtt: %ims\n", get_rtt(main_session));
		}
	}

	class component final : public component_interface
	{
	public:
		void start() override
		{
			command::add("status", []
			{
				scheduler::once(print_status, scheduler::session);
			});

			command::add("rtt", print_rtt);

			if (!game::environment::is_mgo())
			{
				return;
			}

			command::add("kick", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					console::info("Usage: kick <name|index|steam_id>", false);
					return;
				}

				const auto identifier = params.join(1);
				scheduler::once([identifier]
				{
					if (!is_host())
					{
						console::info("Cannot kick as non-host", false);
						return;
					}

					bool is_self{};
					const auto client = find_client(identifier, &is_self);
					if (client == nullptr)
					{
						console::info("Client not found", false);
						return;
					}

					if (is_self)
					{
						console::info("Cannot kick yourself", false);
						return;
					}

					const auto steam_friends = (*game::SteamFriends)();
					game::steam_id steam_id{};
					steam_id.bits = client->sessionUserId->userId;
					const auto name = steam_friends->__vftable->GetFriendPersonaName(steam_friends, steam_id);

					console::info(utils::string::va("%s has been kicked", name), false);

					matchmaking::kick_player_from_lobby(client->sessionUserId->userId);
					matchmaking::ban_player_from_lobby(client->sessionUserId->userId);
					game::fox::nt::Member_::Reset(client);
				}, scheduler::session);
			});
		}
	};
}

REGISTER_COMPONENT(session::component)
