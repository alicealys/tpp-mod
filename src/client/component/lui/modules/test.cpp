#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "../types/ui_menu.hpp"
#include "../main.hpp"
#include "../flow_manager.hpp"
#include "../types/object.hpp"

namespace lui::test
{
	namespace
	{
		ui_element_ptr create_test_menu_1()
		{
			auto menu = ui_menu::create("MENU 1");

			menu->add_button("BUTTON 1", "test2", "description 1");
			menu->add_button("BUTTON 2", {}, "description 2");
			menu->add_button("BUTTON 3", {}, "description 3");
			menu->add_button("BUTTON 4", {}, "description 4");
			menu->add_back_button();
			menu->add_description();

			return menu;
		}

		ui_element_ptr create_test_menu_2()
		{
			auto menu = ui_menu::create("SUB MENU 1");

			menu->add_button("BUTTON 5", {}, "description 5!!");
			menu->add_button("BUTTON 6", {}, "description 6!!");
			menu->add_back_button();
			menu->add_description();

			return menu;
		}
	}

	void initialize()
	{
		flow_manager::register_menu("test1", create_test_menu_1);
		flow_manager::register_menu("test2", create_test_menu_2);
	}
}

#ifdef DEBUG
REGISTER_MODULE(lui::test::initialize)
#endif
