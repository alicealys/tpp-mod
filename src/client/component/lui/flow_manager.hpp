#pragma once

#include "types/ui_element.hpp"

namespace lui::flow_manager
{
	using menu_cb_t = std::function<ui_element_ptr()>;

	void reset();
	void update();

	void register_menu(const std::string& name, const menu_cb_t& menu);

	void request_menu(const std::string& name, const bool popup);
	void request_pop_menu();
	void request_pop_all_menus();

	void load();
}
