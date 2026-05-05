#pragma once

namespace text_chat::ui
{
	void print(const std::string& msg, bool play_sound);

	void add_sound(const unsigned int id, const std::chrono::milliseconds duration);
}
