#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "ui_menu.hpp"
#include "ui_image.hpp"
#include "ui_list.hpp"
#include "ui_text.hpp"
#include "ui_button.hpp"
#include "ui_timer.hpp"

#include "../main.hpp"
#include "../flow_manager.hpp"
#include "../utils.hpp"

namespace lui
{
	ui_menu::ui_menu()
	{
		this->id_ = "uimenu";
	}

	ui_menu_ptr ui_menu::create(const std::string& title_text)
	{
		auto menu = std::make_shared<ui_menu>();
		{
			element_state_t state{};
			state.position.anchor = ANCHOR_ALL;

			element_state_t show = state;
			show.color.a = 1.f;

			element_state_t hide = state;
			hide.color.a = 0.f;
			hide.position.rect.left = -4.f;
			hide.position.rect.right = -4.f;
			hide.position.rect.top = -4.f;
			hide.position.rect.bottom = -4.f;

			menu->register_animation_state("default", state);
			menu->register_animation_state("show", show);
			menu->register_animation_state("hide", hide);

			menu->animate_to_state("hide");
			menu->animate_to_state("show", 200);
		}

		auto bg = ui_image::create();
		{
			element_state_t state{};
			state.position.anchor = ANCHOR_ALL;
			state.color.r = 0.f;
			state.color.g = 0.f;
			state.color.b = 0.f;
			state.color.a = 0.6f;
			bg->register_animation_state("default", state);
			bg->animate_to_state("default");
		}

		auto list_container = ui_element::create();
		{
			element_state_t state{};
			state.position.anchor = ANCHOR_ALL;
			state.position.rect.top = 80.f;
			state.position.rect.left = 92.f;
			state.position.rect.right = 520.f;
			state.position.rect.bottom = 100.f;
			list_container->register_animation_state("default", state);
			list_container->animate_to_state("default");
		}

		menu->list_ = ui_list::create(2.f);
		{
			element_state_t state{};
			state.position.anchor = ANCHOR_ALL;
			menu->list_->register_animation_state("default", state);
			menu->list_->animate_to_state("default");
		}

		auto title = ui_text::create();
		{
			element_state_t state{};
			state.position.anchor = ANCHOR_ALL;
			state.position.rect.top = 50.f;
			state.position.rect.left = 92.f;
			state.position.rect.right = 0.f;
			state.height = 20.f;
			title->register_animation_state("default", state);
			title->animate_to_state("default");
			title->set_text(title_text);
		}

		list_container->add_child(menu->list_);

		menu->add_child(bg);
		menu->add_child(title);
		menu->add_child(list_container);

		menu->set_needs_key_catcher(true);

		utils::play_sound(SOUND_MENU_ENTER);
		menu->register_event_handler("keydown", [menu](ui_element& element, const event_t& event)
		{
			if (event.get<std::int32_t>("key") != VK_ESCAPE)
			{
				return;
			}

			utils::play_sound(SOUND_MENU_LEAVE);
			menu->animate_to_state("hide", 200);
			menu->add_child(ui_timer::create("leave_menu", 200));
		});

		menu->register_event_handler("leave_menu", [](ui_element& element, const event_t& event)
		{
			flow_manager::request_pop_menu();
		});

		menu->register_event_handler("restore_menu", [menu](ui_element& element, const event_t& event)
		{
			menu->animate_to_state("hide");
			menu->animate_to_state("show", 200);
		});

		return menu;
	}

	void ui_menu::add_back_button()
	{
		this->add_button("BACK", [this]()
		{
			utils::play_sound(SOUND_MENU_LEAVE);
			this->animate_to_state("hide", 200);
			this->add_child(ui_timer::create("leave_menu", 200));
		}, "Go back");
	}

	void ui_menu::add_description()
	{
		auto description_container = ui_element::create();
		{
			element_state_t state{};
			state.position.anchor = ANCHOR_TOPLEFT | ANCHOR_RIGHT;
			state.position.rect.top = 8.f;
			state.height = 90.f;

			description_container->register_animation_state("default", state);
			description_container->animate_to_state("default");
		}

		auto description_bg = ui_image::create();
		{
			element_state_t state{};
			state.position.anchor = ANCHOR_ALL;
			state.color.r = 0.f;
			state.color.g = 0.f;
			state.color.b = 0.f;
			state.color.a = 0.7f;

			description_bg->register_animation_state("default", state);
			description_bg->animate_to_state("default");
		}

		this->description_ = ui_text::create();
		{
			element_state_t state{};
			state.position.anchor = ANCHOR_ALL;
			state.position.rect.top = 6.f;
			state.position.rect.left = 6.f;
			state.position.rect.right = 6.f;
			state.position.rect.bottom = 6.f;
			state.height = 20.f;

			this->description_->set_use_stencil(true);
			this->description_->set_use_word_wrapping(true);

			this->description_->register_animation_state("default", state);
			this->description_->animate_to_state("default");
		}

		description_container->add_child(description_bg);
		description_container->add_child(this->description_);
		this->list_->add_child(description_container);
	}

	void ui_menu::add_button(const std::string& text, const button_action_t& action, const std::string& description)
	{
		button_properties_t properties{};
		properties.text = text;
		properties.action = action;

		auto button = ui_button::create(properties);

		button->register_event_handler("gain_focus", [this, description](ui_element& element, const event_t& event)
		{
			if (this->description_ == nullptr)
			{
				return;
			}

			this->description_->set_text(description);
		});

		this->list_->add_child(button);
	}
}
