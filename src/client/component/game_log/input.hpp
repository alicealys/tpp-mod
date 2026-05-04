#pragma once

#include "defs.hpp"

namespace game_log::input
{
	void stop_typing(game_log_state_t& state);

	bool handle_key(const int key, const bool is_down, const bool is_game_console_bind = false);
	bool handle_char(const int key, const bool is_down);

	bool handle_mousewheel(const bool down);

	bool is_input_blocked();
}
