#include <std_include.hpp>

#include "ui_text_input.hpp"

#include "../renderer.hpp"

#include <utils/string.hpp>

namespace lui
{
	std::int64_t ui_text_input::current_focused_id = -1;
	std::int64_t ui_text_input::next_focused_id = -1;
	std::vector<std::int64_t> ui_text_input::active_inputs;

	ui_text_input::ui_text_input()
	{
		this->id_ = "uitextinput";
		this->type_ = UI_TEXT_INPUT;
	}

	ui_text_input::~ui_text_input()
	{
		if (this->input_id_ == -1)
		{
			return;
		}

		for (auto i = ui_text_input::active_inputs.begin(); i != ui_text_input::active_inputs.end(); )
		{
			if (*i == this->input_id_)
			{
				i = ui_text_input::active_inputs.erase(i);
				continue;
			}

			++i;
		}
	}

	void ui_text_input::draw_internal(const draw_info_t& draw_info) const
	{
		const auto& state = this->animation_state_.current_state;

		float color[4]{};
		color[0] = state.color.r;
		color[1] = state.color.g;
		color[2] = state.color.b;
		color[3] = draw_info.alpha;

		float hint_color[4]{};
		hint_color[0] = this->hint_text_color_[0];
		hint_color[1] = this->hint_text_color_[1];
		hint_color[2] = this->hint_text_color_[2];
		hint_color[3] = this->hint_text_color_[3] * draw_info.alpha;

		float outline_color[4]{};
		outline_color[0] = this->outline_color_.r;
		outline_color[1] = this->outline_color_.g;
		outline_color[2] = this->outline_color_.b;
		outline_color[3] = this->outline_color_.a;

		const auto width = (draw_info.rect.right - draw_info.rect.left);
		const auto height = (draw_info.rect.bottom - draw_info.rect.top);

		auto x = draw_info.rect.left;
		auto y = 0.f;

		switch (state.position.vertical_alignment)
		{
		case ALIGN_TOP:
			y = draw_info.rect.top;
			break;
		case ALIGN_MIDDLE:
			y = draw_info.rect.top + height / 2.f - state.height / 2.f;
			break;
		case ALIGN_BOTTOM:
			y = draw_info.rect.bottom - state.height;
			break;
		}

		auto buffer = this->buffer_.data();

		if (buffer[0] == 0)
		{
			renderer::add_draw_text(this->hint_text_.data(), state.height, x, y, hint_color, outline_color, this->formatted_,
				width, height, false, draw_info.rotation);
		}

		if (this->has_focus())
		{
			renderer::add_draw_text_with_cursor(buffer, state.height, x, y, color, outline_color, this->formatted_,
				width, draw_info.rotation, this->cursor_);
		}
		else
		{
			renderer::add_draw_text(buffer, state.height, x, y, color, outline_color, this->formatted_,
				width, height, false, draw_info.rotation);
		}
	}

	void ui_text_input::handle_mouse_button_internal(const mouse_button_params_t& params)
	{
		if (!params.is_down)
		{
			this->set_focused(this->input_state_.was_mouse_in);
		}
	}

	bool ui_text_input::add_char(char c)
	{
		c = utils::string::normalize_ascii_extended(c);
		if (!utils::string::is_char_text(c))
		{
			return false;
		}

		const auto buffer = this->buffer_.data();
		if (std::strlen(buffer) >= this->buffer_.size())
		{
			return false;
		}

		std::memmove(buffer + this->cursor_ + 1, buffer + this->cursor_, this->buffer_.size() - this->cursor_);
		buffer[this->cursor_] = c;
		this->cursor_++;

		if (this->cursor_ == this->buffer_.size())
		{
			buffer[this->cursor_] = 0;
		}

		return true;
	}
	
	bool ui_text_input::handle_paste()
	{
		auto result = false;
		const auto clipboard = utils::string::get_clipboard_data();

		for (auto c : clipboard)
		{
			if (this->cursor_ >= this->buffer_.size())
			{
				return result;
			}

			c = utils::string::normalize_ascii_extended(c);
			if (utils::string::is_char_text(c))
			{
				result |= this->add_char(c);
			}
		}

		return result;
	}

	bool ui_text_input::handle_delete()
	{
		auto result = false;
		auto is_first = true;
		auto buffer = this->buffer_.data();
		while (this->cursor_ > 0)
		{
			const auto is_letter = isalnum(buffer[this->cursor_ - 1]);

			if (is_letter || is_first)
			{
				result |= this->handle_backspace();
			}

			if (!is_letter)
			{
				return result;
			}

			is_first = false;
		}

		return result;
	}

	bool ui_text_input::handle_char(const int key)
	{
		switch (key)
		{
		case 0x1:
			this->clear();
			return true;
		case 0x16:
			return this->handle_paste();
		case 0x7F:
			return this->handle_delete();
		default:
			return this->add_char(static_cast<char>(key));
		}
	}

	void ui_text_input::move_cursor(const bool right)
	{
		auto buffer = this->buffer_.data();
		if (right && buffer[this->cursor_] != 0)
		{
			this->cursor_++;
		}
		else if (!right && this->cursor_ > 0)
		{
			this->cursor_--;
		}
	}

	bool ui_text_input::handle_backspace()
	{
		if (this->cursor_ <= 0)
		{
			return false;
		}

		auto buffer = this->buffer_.data();
		std::memmove(buffer + this->cursor_ - 1, buffer + this->cursor_,
			std::strlen(buffer) + 1 - this->cursor_);
		this->cursor_--;
		return true;
	}

	bool ui_text_input::handle_key(const int key)
	{
		switch (key)
		{
		case VK_LEFT:
			this->move_cursor(false);
			return false;
		case VK_RIGHT:
			this->move_cursor(true);
			return false;
		case VK_ESCAPE:
			this->set_focused(false);
			return false;
		case VK_BACK:
			return this->handle_backspace();
		case VK_TAB:
		{
			const auto next = this->get_next_active_input();
			if (next != -1)
			{
				ui_text_input::next_focused_id = next;
				this->set_focused(false);
			}
			return false;
		}
		case VK_RETURN:
		{
			this->send_input_event("confirm");
			return false;
		}
		}

		return false;
	}

	void ui_text_input::update()
	{
		ui_element::update();
		if (ui_text_input::next_focused_id == this->input_id_)
		{
			ui_text_input::next_focused_id = -1;
			this->set_focused(true);
		}

		if (this->has_focus() && !this->had_focus_)
		{
			event_t event("gain_focus");
			event.target = this->shared_from_this();
			event.immediate = true;
			this->dispatch_event(event);
		}
		else if (!this->has_focus() && this->had_focus_)
		{
			event_t event("lose_focus");
			event.target = this->shared_from_this();
			event.immediate = true;
			this->dispatch_event(event);
		}

		this->had_focus_ = this->has_focus();
	}

	void ui_text_input::send_input_event(const std::string& name)
	{
		const auto len = std::strlen(this->buffer_.data());
		const std::string data = {this->buffer_.data(), len};

		static event_t event;
		event.name = name;
		event.immediate = true;
		event.dispatch_children = false;
		event.target = this->shared_from_this();
		event.params.set("data", data);
		event.params.set("cursor", this->cursor_);

		this->dispatch_event(event);
	}

	ui_text_input_ptr ui_text_input::create()
	{
		auto input = std::make_shared<ui_text_input>();
		input->track();
		input->set_needs_key_catcher(true);
		input->set_handle_mouse(true);

		static std::uint64_t current_id{};
		input->input_id_ = current_id++;

		ui_text_input::active_inputs.emplace_back(input->input_id_);

		input->buffer_.resize(max_input_len);
		input->cursor_ = 0;

		input->hint_text_color_[0] = 1.f;
		input->hint_text_color_[1] = 1.f;
		input->hint_text_color_[2] = 1.f;
		input->hint_text_color_[3] = 0.5f;

		return input;
	}

	void ui_text_input::handle_key_event_internal(const key_event_t& params)
	{
		if (!this->has_focus())
		{
			return;
		}

		if (params.is_char && this->handle_char(params.key))
		{
			this->send_input_event("input");
		}
		else if (!params.is_mousewheel && params.is_down && this->handle_key(params.key))
		{
			this->send_input_event("input");
		}
	}

	void ui_text_input::set_input(const std::string& data)
	{
		this->cursor_ = 0;
		strncpy_s(this->buffer_.data(), this->buffer_.size(), data.data(), _TRUNCATE);
	}

	std::string ui_text_input::get_input()
	{
		return this->buffer_;
	}

	void ui_text_input::set_cursor(const std::int32_t cursor)
	{
		if (cursor < this->buffer_.size() && cursor > 0)
		{
			this->cursor_ = cursor;
		}
	}

	std::int32_t ui_text_input::get_cursor()
	{
		return this->cursor_;
	}

	void ui_text_input::set_hint_text(const std::string& text)
	{
		this->hint_text_ = text;
	}

	void ui_text_input::clear()
	{
		this->cursor_ = 0;
		std::memset(this->buffer_.data(), 0, this->buffer_.size());
	}

	void ui_text_input::set_focused(const bool focused)
	{
		if (!focused && this->has_focus())
		{
			ui_text_input::current_focused_id = -1;
		}
		else if (focused)
		{
			ui_text_input::current_focused_id = this->input_id_;
		}
	}

	bool ui_text_input::has_focus() const
	{
		return ui_text_input::current_focused_id == this->input_id_;
	}

	std::int64_t ui_text_input::get_next_active_input()
	{
		for (auto i = 0ull; i < ui_text_input::active_inputs.size(); i++)
		{
			if (ui_text_input::active_inputs[i] == this->input_id_)
			{
				const auto idx = (i + 1) % ui_text_input::active_inputs.size();
				return ui_text_input::active_inputs[idx];
			}
		}

		return -1;
	}
}
