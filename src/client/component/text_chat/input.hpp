#pragma once

#include "text_chat.hpp"

namespace text_chat::input
{
	void stop_typing(chat_state_t& state);

	bool handle_key(const int key, const bool is_down, const bool is_game_console_bind);
	bool handle_mousewheel(const bool down);

	bool is_input_blocked();
}
