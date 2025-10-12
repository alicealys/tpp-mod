#pragma once

#include "../console.hpp"
#include "../command.hpp"
#include "../vars.hpp"
#include "../scheduler.hpp"

#include <utils/concurrency.hpp>

namespace game_log
{
	extern vars::var_ptr var_chat_enable;
	extern vars::var_ptr var_console_enable;

	extern vars::var_ptr var_game_log_message_time;
	extern vars::var_ptr var_game_log_input_width;
	extern vars::var_ptr var_game_log_input_bg;
	extern vars::var_ptr var_game_log_output_bg;
	extern vars::var_ptr var_game_log_scale;
	extern vars::var_ptr var_game_log_offset;
	extern vars::var_ptr var_game_log_line_spacing;
	extern vars::var_ptr var_game_log_height;
	extern vars::var_ptr var_game_log_width;
	extern vars::var_ptr var_game_log_max_message_len;

	constexpr auto game_log_history_size = 40;
	constexpr auto game_log_view_size = 8;
	constexpr auto game_log_view_index_begin = 2;
	constexpr auto game_log_message_input_index = 0;
	constexpr auto game_log_cursor_interval = 500ull;
	constexpr auto game_log_message_sound_id = 22;
	constexpr auto game_log_message_fade_time = 1000;
	constexpr auto game_log_scroll_sound_id = 152;
	constexpr auto game_log_cursor_char = '_';
	constexpr auto game_log_key_default = 'Y';

	extern char chat_input_prefix[];
	extern char console_input_prefix[];

	using message_buffer_t = char[256];

	struct game_log_message_t
	{
		message_buffer_t buffer;
		std::chrono::high_resolution_clock::time_point time;
	};

	struct sound_play_t
	{
		unsigned int id;
		bool started;
		std::chrono::high_resolution_clock::time_point start;
		std::chrono::milliseconds duration;
	};

	enum game_log_input_mode_t
	{
		mode_none,
		mode_chat,
		mode_console,
	};

	struct game_log_state_t
	{
		std::deque<game_log_message_t> messages;
		int chat_offset;
		float view_text_offset_x;
		bool is_shift_down;
		int message_index;
		bool is_typing;
		game_log_input_mode_t mode;
		message_buffer_t input;
		int cursor;
		std::vector<sound_play_t> sounds;
		std::deque<std::string> history;
		std::int32_t history_index = -1;
	};

	extern utils::concurrency::container<game_log_state_t, std::recursive_mutex> game_log_state;

	bool is_chat_enabled();
	bool is_console_enabled();
	bool can_show_game_log();
	bool can_use_chat();

	bool is_char_text(char c);
	bool check_message(const std::string& msg);

	void reset_log();

	bool is_initialized();
}
