#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "ui_button.hpp"
#include "ui_text.hpp"
#include "ui_image.hpp"
#include "../main.hpp"
#include "../flow_manager.hpp"
#include "../utils.hpp"

namespace lui
{
	ui_button_ptr ui_button::create(const button_properties_t& properties)
	{
		auto button = std::make_shared<ui_button>();

		button->set_handle_mouse(true);

		button->register_event_handler("mousedown", [properties](ui_element& element, const event_t& event)
		{
			utils::play_sound(SOUND_SELECT);

			const auto action_type = properties.action.index();
			switch (action_type)
			{
			case 1:
			{
				const auto& menu = std::get<std::string>(properties.action);
				flow_manager::request_menu(menu, false);
				break;
			}
			case 2:
			{
				const auto& callback = std::get<std::function<void()>>(properties.action);
				try
				{
					callback();

				}
				catch (const std::exception& e)
				{
					console::error("LUI: error executing button callback: %s\n", e.what());
				}

				break;
			}
			}
		});

		switch (properties.style)
		{
		case BUTTON_STYLE_DEFAULT:
		default:
			button->button_style_default(properties);
			break;
		}

		return button;
	}

	void ui_button::button_style_default(const button_properties_t& properties)
	{
		{
			element_state_t state{};
			state.position.anchor = ANCHOR_TOPLEFT | ANCHOR_RIGHT;
			state.position.rect.left = 0.f;
			state.position.rect.top = 0.f;
			state.height = 27.f;

			element_state_t focused = state;
			focused.position.rect.left -= 3.f;
			focused.position.rect.top -= 2.f;
			focused.width += 4.f;

			this->register_animation_state("default", state);
			this->register_animation_state("focus", focused);
			this->register_animation_state("unfocus", state);

			this->animate_to_state("default");
		}

		auto text_element = ui_text::create();
		{
			element_state_t state{};
			state.position.anchor = ANCHOR_ALL;
			state.position.vertical_alignment = ALIGN_MIDDLE;
			state.color.r = 0.8f;
			state.color.g = 0.8f;
			state.color.b = 0.8f;
			state.color.a = 0.8f;
			state.position.rect.left = 10.f;
			state.height = 20.f;

			element_state_t focused = state;
			focused.color.r = 0.f;
			focused.color.g = 0.f;
			focused.color.b = 0.f;
			focused.color.a = 1.f;

			text_element->register_animation_state("default", state);
			text_element->register_animation_state("focus", focused);
			text_element->register_animation_state("unfocus", state);

			text_element->animate_to_state("default");
			text_element->set_text(properties.text);
		}

		auto bg = ui_image::create();
		{
			element_state_t state{};
			state.position.anchor = ANCHOR_ALL;
			state.color.r = 0.f;
			state.color.g = 0.f;
			state.color.b = 0.f;
			state.color.a = 0.7f;

			element_state_t focused = state;
			focused.color.r = 1.f;
			focused.color.g = 1.f;
			focused.color.b = 1.f;

			bg->register_animation_state("default", state);
			bg->register_animation_state("focus", focused);
			bg->register_animation_state("unfocus", state);

			bg->animate_to_state("default");
		}

		this->add_child(bg);
		this->add_child(text_element);

		this->register_event_handler("mouseenter", [bg, text_element](ui_element& element, const event_t& event)
		{
			element.dispatch_event("gain_focus");
			element.animate_to_state("focus", 50);
			text_element->animate_to_state("focus");
			bg->animate_to_state("focus");
			utils::play_sound(SOUND_SCROLL);
		});

		this->register_event_handler("mouseleave", [bg, text_element](ui_element& element, const event_t& event)
		{
			element.dispatch_event("lose_focus");
			element.animate_to_state("unfocus", 50);
			text_element->animate_to_state("unfocus");
			bg->animate_to_state("unfocus");
		});
	}
}
