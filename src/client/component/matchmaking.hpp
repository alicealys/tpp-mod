#pragma once

namespace matchmaking
{
	enum event_callback_t
	{
		event_create_lobby = 0,
		event_join_lobby = 1,
		event_leave_lobby = 2,
	};

	game::steam_id get_current_steam_id();

	void set_lobby_data(const std::string& key, const std::string& value);
	void set_lobby_data(const std::string& key, const std::uint64_t value);
	const char* get_lobby_data(const std::string& key);

	void ban_player_from_lobby(const std::uint64_t steam_id);
	void unban_player_from_lobby(const std::uint64_t steam_id);
	void kick_player_from_lobby(const std::uint64_t steam_id);

	void connect_to_lobby(game::steam_id lobby_id);

	void register_callback(const std::uint32_t type, const std::function<void(game::mgo_match_t*, const game::steam_id)> callback);
}
