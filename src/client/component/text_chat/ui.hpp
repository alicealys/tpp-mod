#pragma once

namespace text_chat::ui
{
	void print(const std::wstring& msg, bool play_sound);

	void add_sound(const unsigned int id, const std::chrono::milliseconds duration);
}
