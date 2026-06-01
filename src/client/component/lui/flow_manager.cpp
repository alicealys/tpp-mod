#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "flow_manager.hpp"
#include "main.hpp"

#include "../command.hpp"

namespace lui::flow_manager
{
	namespace
	{
		struct menu_entry_t
		{
			std::string name;
			bool popup;
			menu_cb_t callback;
			ui_element_ptr root;
		};

		struct
		{
			std::deque<menu_entry_t> menu_stack;
			std::vector<menu_entry_t> requested_menus;
			std::unordered_map<std::string, menu_cb_t> menus;
			std::atomic_bool restart_requested = true;
			std::atomic_bool key_catcher_enabled = false;
			bool requested_pop_menu{};
			bool requested_pop_all_menus{};
			bool needs_key_catcher = false;
		} state{};

		ui_element_ptr try_create_menu(const std::string& name, menu_cb_t& callback)
		{
			try
			{
				const auto menu = callback();
				if (menu == nullptr)
				{
					return nullptr;
				}

				return menu;
			}
			catch (const std::exception& e)
			{
				console::error("LUI: error opening menu \"%s\": %s\n", name.data(), e.what());
				return nullptr;
			}
		}

		bool open_menu(menu_entry_t& menu_entry)
		{
			const auto iter = state.menus.find(menu_entry.name);
			if (iter == state.menus.end())
			{
				return false;
			}

			auto& root = get_root_element();

			const auto menu = try_create_menu(menu_entry.name, iter->second);
			if (menu == nullptr)
			{
				return false;
			}

			menu_entry.root = menu;
			menu_entry.callback = iter->second;

			if (!menu_entry.popup)
			{
				root->remove_all_children();
			}

			root->add_child(menu);
			return true;
		}

		void pop_menu()
		{
			if (state.menu_stack.empty())
			{
				return;
			}

			auto& top = state.menu_stack.back();
			state.menu_stack.pop_back();

			auto& root = get_root_element();

			root->remove_child(top.root);
			top.root->close();

			if (top.popup)
			{
				return;
			}

			if (!state.menu_stack.empty())
			{
				auto& back = state.menu_stack.back();
				if (!open_menu(back))
				{
					pop_menu();
				}

				back.root->dispatch_event("restore_menu");
			}
		}

		void pop_all_menus()
		{
			while (!state.menu_stack.empty())
			{
				auto& top = state.menu_stack.back();
				top.root->close();
				state.menu_stack.pop_back();
			}
		}
	}

	void initialize()
	{
		state.menu_stack.clear();
		state.menus.clear();
	}

	void update()
	{
		if (state.requested_pop_menu && !state.requested_pop_all_menus)
		{
			pop_menu();
			state.requested_pop_menu = false;
		}
		else if (state.requested_pop_all_menus)
		{
			pop_all_menus();
		}

		for (auto& menu : state.requested_menus)
		{
			if (open_menu(menu))
			{
				state.menu_stack.push_back(menu);
			}
		}

		state.requested_pop_menu = false;
		state.requested_pop_all_menus = false;
		state.requested_menus.clear();
	}

	void register_menu(const std::string& name, const menu_cb_t& menu)
	{
		state.menus[name] = menu;
	}

	void request_menu(const std::string& name, const bool popup)
	{
		menu_entry_t menu_entry{};
		menu_entry.name = name;
		menu_entry.popup = popup;
		state.requested_menus.emplace_back(menu_entry);
	}

	void request_pop_menu()
	{
		state.requested_pop_menu = true;
	}

	void request_pop_all_menus()
	{
		state.requested_pop_all_menus = true;
	}

	void load()
	{
		command::add("lui_open", [](const command::params& params)
		{
			const auto name = params.get(1);
			request_menu(name, false);
		});

		command::add("lui_open_popup", [](const command::params& params)
		{
			const auto name = params.get(1);
			request_menu(name, true);
		});

		command::add("lui_close", request_pop_menu);
		command::add("lui_close_all", request_pop_all_menus);
	}
}
