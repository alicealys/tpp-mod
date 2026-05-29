#pragma once

namespace lui::input
{
	void update();
	void post_update();

	void handle_mousemove(HWND hwnd);
	void handle_mouse_button(const int button, const bool is_down);
	bool handle_mouse_wheel(const bool down);

	bool handle_char(const int key, const bool is_down, const bool is_game_console_bind = false);
	bool handle_key(const int key, const bool is_down, const bool is_game_console_bind = false);
	bool is_key_catcher_enabled();
	void set_key_catcher();
}
