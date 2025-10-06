#pragma once

namespace game_log::ui
{
	void print(const std::string& msg, bool play_sound);

	void add_sound(const unsigned int id, const std::chrono::milliseconds duration);
}
