#pragma once

#include "ui_element.hpp"
#include "ui_button.hpp"
#include "ui_list.hpp"
#include "ui_text.hpp"

namespace lui
{
	class ui_menu;
	using ui_menu_ptr = std::shared_ptr<ui_menu>;

	class ui_menu : public ui_element
	{
	public:
		static ui_menu_ptr create(const std::string& name);

		void add_description();
		void add_back_button();
		void add_button(const std::string& text, const button_action_t& action = {}, const std::string& description = {});

	private:
		ui_list_ptr list_;
		ui_text_ptr description_;

	};
}
