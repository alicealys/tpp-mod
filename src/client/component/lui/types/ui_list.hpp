#pragma once

#include "ui_element.hpp"

namespace lui
{
	class ui_list;
	using ui_list_ptr = std::shared_ptr<ui_list>;

	class ui_list : public ui_element
	{
	public:
		ui_list();

		static ui_list_ptr create(const float spacing);

		void add_child(const ui_element_ptr& child);
		void remove_child(const ui_element_ptr& child);
		void remove_all_children();

	private:
		void update_list();

		float spacing_{};

	};
}
