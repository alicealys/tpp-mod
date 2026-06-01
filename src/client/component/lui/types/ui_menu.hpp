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
		ui_menu();

		static ui_menu_ptr create(const std::string& name);

		void close(bool recurse = true) override;

		ui_text_ptr add_description();
		ui_button_ptr add_back_button();
		ui_button_ptr add_button(const std::string& text, const button_action_t& action = {}, const std::string& description = {});

	private:
		ui_list_ptr list_{};
		ui_text_ptr description_{};

	};
}
