#pragma once

namespace matchmaking
{
	void ban_player_from_lobby(const std::uint64_t steam_id);
	void kick_player_from_lobby(const std::uint64_t steam_id);
	void clear_kicks();

	void connect_to_lobby(game::steam_id lobby_id);
}
