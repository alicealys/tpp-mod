#pragma once

namespace dedicated_server
{
	void ban_player_from_session(const game::steam_id steam_id);
	void unban_player_from_session(const game::steam_id steam_id);
}
