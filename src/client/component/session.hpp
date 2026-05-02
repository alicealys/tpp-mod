#pragma once

namespace session
{
	int get_rtt(game::fox::nt::impl::SessionImpl2* session);
	void connect_to_lobby(const game::steam_id lobby_id);
}
