#pragma once

#include "ui_element.hpp"

namespace lui
{
	class ui_text;
	using ui_text_ptr = std::shared_ptr<ui_text>;

	class ui_text : public ui_element
	{
	public:
		void set_text(const std::string& text);
		void set_outline_color(const float r, const float g, const float b, const float a);

		void set_formatted(const bool enabled);
		void set_use_stencil(const bool enabled);
		void set_use_word_wrapping(const bool enabled);

		static ui_text_ptr create();

	private:
		void draw_internal(const draw_info_t& inherit) const override;

		std::string text_;
		color_t outline_color_{};
		bool formatted_;
		bool use_stencil_;
		bool use_word_wrapping_;
		std::optional<std::function<const char* ()>> get_display_text_callback;

	};
}
