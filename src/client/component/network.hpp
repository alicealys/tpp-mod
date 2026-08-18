#pragma once

#include "game/game.hpp"

namespace network
{
	void unban_player(const game::steam_id steam_id);
	void ban_player(const game::steam_id steam_id);
}
