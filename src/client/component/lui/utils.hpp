#pragma once

#include "game/game.hpp"

namespace lui
{
	enum sound_id_t : std::uint32_t
	{
		SOUND_MENU_ENTER = 3,
		SOUND_MENU_LEAVE = 4,
		SOUND_SCROLL = 8,
		SOUND_SELECT = 9,
		SOUND_CANCEL = 10,
	};
}

namespace lui::utils
{
	game::fox::gr::ResourceId get_material_resource(const std::uint64_t hash);
	game::fox::gr::ResourceId get_material_resource(const std::string& path);

	game::fox::gr::ResourceId get_texture_resource(const std::uint64_t hash);
	game::fox::gr::ResourceId get_texture_resource(const std::string& path);

	void play_sound(const std::uint32_t id);
}
