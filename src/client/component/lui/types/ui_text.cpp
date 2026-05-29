#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "ui_text.hpp"
#include "../main.hpp"
#include "../renderer.hpp"
#include "../../renderer.hpp"

namespace lui
{
	void ui_text::draw_internal(const draw_info_t& draw_info) const
	{
		const auto& state = this->animation_state_.current_state;

		float color[4]{};
		color[0] = state.color.r;
		color[1] = state.color.g;
		color[2] = state.color.b;
		color[3] = draw_info.alpha;

		float outline_color[4]{};
		outline_color[0] = this->outline_color_.r;
		outline_color[1] = this->outline_color_.g;
		outline_color[2] = this->outline_color_.b;
		outline_color[3] = this->outline_color_.a;

		const auto width = (draw_info.rect.right - draw_info.rect.left);
		[[ maybe_unused ]] const auto height = (draw_info.rect.bottom - draw_info.rect.top);

		const auto text = get_display_text_callback.has_value()
			? get_display_text_callback->operator()()
			: this->text_.data();

		const auto text_width = ::renderer::calc_text_width(text, state.height, this->formatted_);

		auto x = 0.f;

		switch (state.position.alignment)
		{
		case ALIGN_LEFT:
			x = draw_info.rect.left;
			break;
		case ALIGN_CENTER:
			x = draw_info.rect.left + width / 2.f - text_width / 2.f;
			break;
		case ALIGN_RIGHT:
			x = draw_info.rect.right - text_width;
			break;
		}

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

		auto display_width = this->use_stencil_ || this->use_word_wrapping_
			? draw_info.rect.right - draw_info.rect.left
			: 0.f;

		auto display_height = this->use_stencil_ || this->use_word_wrapping_
			? draw_info.rect.bottom - draw_info.rect.top
			: 0.f;

		renderer::add_draw_text(text, state.height, x, y, color, outline_color, this->formatted_, display_width, display_height, this->use_word_wrapping_);
	}

	void ui_text::set_text(const std::string& text)
	{
		this->text_ = text;
	}

	void ui_text::set_outline_color(const float r, const float g, const float b, const float a)
	{
		this->outline_color_.r = r;
		this->outline_color_.g = g;
		this->outline_color_.b = b;
		this->outline_color_.a = a;
	}

	void ui_text::set_formatted(const bool formatted)
	{
		this->formatted_ = formatted;
	}

	void ui_text::set_use_stencil(const bool enabled)
	{
		this->use_stencil_ = enabled;
	}

	void ui_text::set_use_word_wrapping(const bool enabled)
	{
		this->use_word_wrapping_ = enabled;
	}

	ui_text_ptr ui_text::create()
	{
		return std::make_shared<ui_text>();
	}
}
