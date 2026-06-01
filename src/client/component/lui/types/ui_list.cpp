#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "ui_list.hpp"
#include "ui_image.hpp"
#include "../main.hpp"

namespace lui
{
	ui_list::ui_list()
	{
		this->id_ = "uilist";
		this->type_ = UI_LIST;
	}

	ui_list_ptr ui_list::create(const float spacing)
	{
		auto list = std::make_shared<ui_list>();
		list->track();
		list->spacing_ = spacing;
		return list;
	}

	void ui_list::add_child(const ui_element_ptr& child)
	{
		if (child == this->shared_from_this())
		{
			return;
		}

		auto container = ui_element::create();
		container->add_child(child);
		ui_element::add_child(container);
		this->update_list();
	}

	void ui_list::remove_child(const ui_element_ptr& child)
	{
		if (child == this->shared_from_this())
		{
			return;
		}

		if (child->parent_.lock() == this->shared_from_this())
		{
			ui_element::remove_child(child);
			this->update_list();
			return;
		}

		for (auto& container : this->children_)
		{
			if (container->get_first_child() == child)
			{
				ui_element::remove_child(container);
			}
		}

		this->update_list();
	}

	void ui_list::remove_all_children()
	{
		ui_element::remove_all_children();
		this->update_list();
	}

	void ui_list::update_list()
	{
		float offset = 0.f;

		for (auto& container : this->children_)
		{
			const auto element = container->get_first_child();

			rect_t rect{};
			element->calculate_rect(rect, rect);

			element_state_t container_state{};
			container_state.position.anchor = ANCHOR_TOPLEFT | ANCHOR_RIGHT;
			container_state.position.rect.top = offset;
			container_state.height = (rect.bottom - rect.top);

			container->register_animation_state("default", container_state);
			container->animate_to_state("default");

			offset += container_state.height + this->spacing_;
		}
	}
}
