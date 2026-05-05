#pragma once

#include "../console.hpp"
#include "../command.hpp"
#include "../vars.hpp"
#include "../scheduler.hpp"

#include <utils/concurrency.hpp>

namespace text_chat
{
	extern vars::var_ptr var_chat_enable;
	extern vars::var_ptr var_console_enable;

	extern vars::var_ptr var_chat_message_time;
	extern vars::var_ptr var_chat_input_bg;
	extern vars::var_ptr var_chat_output_bg;
	extern vars::var_ptr var_chat_offset;
	extern vars::var_ptr var_chat_height;
	extern vars::var_ptr var_chat_width;
	extern vars::var_ptr var_chat_scale;

	constexpr auto chat_history_size = 40;
	constexpr auto chat_message_sound_id = 22;
	constexpr auto chat_message_fade_time = 1000;
	constexpr auto chat_scroll_sound_id = 152;
	constexpr auto chat_key_default = 'Y';
	constexpr auto chat_message_max_len = 200;

	using message_buffer_t = char[256];

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

	enum chat_input_mode_t
	{
		mode_none,
		mode_chat,
	};

	struct chat_state_t
	{
		std::deque<chat_message_t> messages;
		float view_text_offset_y;
		int message_index;
		bool is_typing;
		chat_input_mode_t mode;
		message_buffer_t input;
		int cursor;
		std::vector<sound_play_t> sounds;
		std::deque<std::string> history;
		std::int32_t history_index = -1;
	};

	extern utils::concurrency::container<chat_state_t, std::recursive_mutex> chat_state;

	bool is_chat_enabled();
	bool can_use_chat();

	std::string clean_message(const std::string& msg);

	void clear();

	bool is_initialized();
}
