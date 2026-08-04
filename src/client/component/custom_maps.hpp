#pragma once

#include "game/game.hpp"
#include "mods.hpp"

namespace custom_maps
{
	std::string get_current_map_name();

	void on_lobby_create(game::mgo_match_t* match, game::steam_id lobby_id);
	void sync_registered_maps(const game::steam_id lobby_id);
	bool check_download_map(const game::steam_id lobby_id, std::string& error,
		const std::string& base_url, std::vector<mods::mod_download_file_t>& files);

	void register_usermaps();
	void register_maps();

	std::uint8_t get_map_id(const std::string& name);
	std::string get_map_name(const std::uint8_t index);
	std::uint8_t get_map_count();
}
