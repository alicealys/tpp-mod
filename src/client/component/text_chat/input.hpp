#pragma once

#include "text_chat.hpp"

namespace text_chat::input
{
	void stop_typing(chat_state_t& state);
	void handle_typing(UINT msg, WPARAM w_param, LPARAM l_param);
	bool is_input_blocked();
}
