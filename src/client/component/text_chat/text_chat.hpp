#pragma once

#include "../console.hpp"
#include "../command.hpp"
#include "../vars.hpp"
#include "../scheduler.hpp"

#include <utils/concurrency.hpp>

namespace text_chat
{
	extern vars::var_ptr var_chat_time;
	extern vars::var_ptr var_chat_key;
	extern vars::var_ptr var_chat_enable;

	constexpr auto chat_history_size = 20;
	constexpr auto chat_size = 5; // do not change
	constexpr auto chat_view_size = 4; // ^
	constexpr auto chat_message_input_index = 4; // ^
	constexpr auto chat_message_max_len = 50;
	constexpr auto chat_cursor_interval = 500;
	constexpr auto chat_message_msg_id = 20;
	constexpr auto chat_message_sound_id = 150;
	constexpr auto chat_scroll_sound_id = 152;
	constexpr auto chat_cursor_char = '_';
	constexpr auto chat_key_default = 'Y';

	extern char chat_input_prefix[];

	using message_buffer_t = char[128];

	struct chat_message_t
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

	struct chat_state_t
	{
		std::deque<chat_message_t> messages;
		int chat_offset;
		int message_index;
		bool is_typing;
		bool block_input;
		message_buffer_t input;
		int cursor;
		bool need_play_sound;
		std::vector<sound_play_t> sounds;
	};

	extern utils::concurrency::container<chat_state_t, std::recursive_mutex> chat_state;

	bool is_chat_enabled();
	bool can_use_chat();

	bool is_char_text(char c);
	bool check_message(const std::string& msg);

	void reset_chat();
}
