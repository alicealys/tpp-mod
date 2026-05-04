#pragma once

namespace game_console
{
	bool handle_key(const int key, const bool is_down, const bool is_console_bind);
	bool handle_char(const int key, const bool is_down);

	bool handle_mousewheel(const bool down);

	void print(const std::string& data);
}
