#pragma once

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
	void play_sound(const std::uint32_t id);
}
