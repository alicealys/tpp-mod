#pragma once

#include "game/game.hpp"

namespace utils::steam
{
	template <typename T>
	T get_lobby_data(const game::steam_id lobby_id, const char* key, const std::int32_t index = -1, const std::int32_t limit = -1);

	void set_lobby_data(const game::steam_id lobby_id, const char* key, const std::string& value, const std::int32_t index = -1);
	void set_lobby_data(const game::steam_id lobby_id, const char* key, const char* value, const std::int32_t index = -1);

	template <typename T>
	void set_lobby_data(const game::steam_id lobby_id, const char* key, const T& value, const std::int32_t index = -1)
	{
		set_lobby_data(lobby_id, key, std::to_string(value), index);
	}
}
