#pragma once

#include "ui_text.hpp"

namespace lui
{
	class ui_text_input;
	using ui_text_input_ptr = std::shared_ptr<ui_text_input>;

	constexpr const auto max_input_len = 0x100;

	class ui_text_input : public ui_text
	{
	public:
		static ui_text_input_ptr create();

		ui_text_input();
		~ui_text_input();

		void set_input(const std::string& data);
		std::string get_input();

		void set_cursor(const std::int32_t cursor);
		std::int32_t get_cursor();

		void set_hint_text(const std::string& text);

		void set_focused(const bool focused);

		void clear();

	private:
		void update() override;
		void draw_internal(const draw_info_t& draw_info) const override;

		void handle_mouse_button_internal(const mouse_button_params_t& params) override;
		void handle_key_event_internal(const key_event_t& event) override;

		bool handle_char(const int key);
		bool handle_key(const int key);
		bool handle_paste();
		bool handle_delete();
		bool handle_backspace();

		bool add_char(char c);
		void move_cursor(const bool right);

		void send_input_event(const std::string& name);

		std::int64_t get_next_active_input();
		bool has_focus() const;

		float hint_text_color_[4]{};
		std::string hint_text_{};
		std::string buffer_{};
		std::int32_t cursor_{};
		std::int64_t input_id_ = -1;
		bool had_focus_{};

		static std::int64_t current_focused_id;
		static std::int64_t next_focused_id;
		static std::vector<std::int64_t> active_inputs;

	};
}
