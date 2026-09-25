#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "console.hpp"
#include "scheduler.hpp"
#include "game_socket.hpp"

namespace steam_player_history
{
	namespace
	{
		void member_event_handler(game::fox::nt::impl::SessionImpl2 *session, std::uint8_t *arg)
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
			const auto steam_friends = (*game::SteamFriends)();
			steam_friends->__vftable->SetPlayedWith(steam_friends, user);
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
		}

		void start() override
		{
			game_socket::on_session_notify(game::fox::nt::NOTIFY_JOIN_MEMBER, member_event_handler);
		}
	};
}

REGISTER_COMPONENT(steam_player_history::component)
