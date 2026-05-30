#include <std_include.hpp>

#include "main.hpp"
#include "scripting.hpp"
#include "flow_manager.hpp"

#include "types/ui_button.hpp"
#include "types/ui_element.hpp"
#include "types/ui_image.hpp"
#include "types/ui_list.hpp"
#include "types/ui_menu.hpp"
#include "types/ui_text.hpp"
#include "types/ui_timer.hpp"

#pragma warning(push)
#pragma warning(disable: 4459)
#pragma warning(disable: 4702)
#pragma warning(disable: 5321)

#define SOL_ALL_SAFETIES_ON 1
#define SOL_PRINT_ERRORS 0
#include <sol/sol.hpp>

#include <utils/io.hpp>

namespace lui::scripting
{
	namespace
	{
		sol::state scripting_state;

		sol::lua_value convert(lua_State* state, const object_value& value)
		{
			switch (value.get_type())
			{
			case object_value::VALUE_TYPE_BOOL:
				return {state, value.as<bool>()};
			case object_value::VALUE_TYPE_INTEGER:
				return {state, value.as<std::int32_t>()};
			case object_value::VALUE_TYPE_FLOAT:
				return {state, value.as<float>()};
			case object_value::VALUE_TYPE_STRING:
				return {state, value.as<std::string>()};
			}

			return {state, sol::lua_nil};
		}

		object_value convert(const sol::lua_value& value)
		{
			if (value.is<bool>())
			{
				return value.as<bool>();
			}

			if (value.is<std::int32_t>())
			{
				return value.as<std::int32_t>();
			}

			if (value.is<float>())
			{
				return value.as<float>();
			}

			if (value.is<std::string>())
			{
				return value.as<std::string>();
			}

			return {};
		}

		void load_scripts(const std::string& script_dir)
		{
			if (!utils::io::directory_exists(script_dir))
			{
				return;
			}

			const auto scripts = utils::io::list_files(script_dir);

			for (const auto& script : scripts)
			{
				if (!script.ends_with(".lua"))
				{
					continue;
				}

				try
				{
					scripting_state.safe_script_file(script);

				}
				catch (const std::exception& e)
				{
					printf("LUI: error loading script \"%s\": %s\n", script.data(), e.what());
				}
			}
		}

		template <typename T>
		auto register_base_methods(sol::state& state, T usertype)
		{
			usertype["getid"] = &ui_element::get_id;
			usertype["setid"] = &ui_element::set_id;

			usertype[sol::meta_function::index] = [](const ui_element& element, const sol::this_state s,
				const std::string& key)
			{
				if (!element.metadata.contains(key))
				{
					return sol::lua_value{s, sol::lua_nil};
				}

				return convert(s, element.metadata.at(key));
			};

			usertype[sol::meta_function::new_index] = [](ui_element& element, const sol::this_state s,
				const std::string& key, const sol::lua_value& value)
			{
				const auto converted = convert(value);
				element.metadata.set(key, converted);
			};

			usertype["removeallchildren"] = &ui_element::remove_all_children;
			usertype["close"] = &ui_element::close;

			usertype["getfirstchild"] = &ui_element::get_first_child;
			usertype["getlastchild"] = &ui_element::get_last_child;
			usertype["getallchildren"] = &ui_element::get_all_children;
			usertype["getfirstdescendantbyid"] = &ui_element::get_first_descendant_by_id;

			usertype["sethandlemouse"] = &ui_element::set_handle_mouse;
			usertype["setneedskeycatcher"] = &ui_element::set_needs_key_catcher;
			usertype["ismousein"] = &ui_element::is_mouse_in;
			usertype["makedraggable"] = &ui_element::make_draggable;

			usertype["addchild"] = [](ui_element& element, ui_element& child)
			{
				element.add_child(child.shared_from_this());
			};

			usertype["removechild"] = [](ui_element& element, ui_element& child)
			{
				element.remove_child(child.shared_from_this());
			};

			usertype["registeranimationstate"] = [](ui_element& element, const std::string& name, const sol::table& state)
			{
				element_state_t element_state{};

				element_state_t current_state{};
				element.get_animation_state(name, current_state);

				auto anchor = current_state.position.anchor;

				const auto set_flag = [&](const std::string& name, const std::uint32_t flag)
				{
					if (!state[name].is<bool>())
					{
						return;
					}

					const auto value = state.get<bool>(name);
					if (value)
					{
						anchor |= flag;
					}
					else
					{
						anchor &= ~flag;
					}
				};

				set_flag("leftanchor", ANCHOR_LEFT);
				set_flag("topanchor", ANCHOR_TOP);
				set_flag("rightanchor", ANCHOR_RIGHT);
				set_flag("bottomanchor", ANCHOR_BOTTOM);

				element_state.position.anchor = anchor;
				element_state.position.rect.top = state.get_or("top", current_state.position.rect.top);
				element_state.position.rect.left = state.get_or("left", current_state.position.rect.left);
				element_state.position.rect.bottom = state.get_or("bottom", current_state.position.rect.bottom);
				element_state.position.rect.right = state.get_or("right", current_state.position.rect.right);
				element_state.width = state.get_or("width", current_state.width);
				element_state.height = state.get_or("height", current_state.height);
				element_state.position.rotation = state.get_or("rotation", current_state.position.rotation);
				element_state.position.alignment = state.get_or("rotation", current_state.position.alignment);
				element_state.position.vertical_alignment = state.get_or("rotation", current_state.position.vertical_alignment);

				auto color = state["color"];
				if (color.is<sol::table>())
				{
					element_state.color.r = color["r"].get_or(current_state.color.r);
					element_state.color.g = color["g"].get_or(current_state.color.g);
					element_state.color.b = color["b"].get_or(current_state.color.b);
					element_state.color.a = color["a"].get_or(current_state.color.a);
				}

				element.register_animation_state(name, element_state);
			};

			usertype["animatetostate"] = sol::overload(
				[](ui_element& element, const std::string& state)
				{
					element.animate_to_state(state);
				},
				[](ui_element& element, const std::string& state, const std::uint32_t duration)
				{
					element.animate_to_state(state, duration);
				},
				[](ui_element& element, const std::string& state, const std::uint32_t duration, const std::uint32_t mode)
				{
					element.animate_to_state(state, duration, mode);
				}
			);

			usertype["getrect"] = [&state](ui_element& element)
			{
				auto result = state.create_table();

				rect_t rect{};
				element.get_rect(rect);

				result["left"] = rect.left;
				result["right"] = rect.right;
				result["top"] = rect.top;
				result["bottom"] = rect.bottom;

				return result;
			};

			usertype["setrect"] = [](ui_element& element, const sol::table& rect_table)
			{
				rect_t rect{};

				rect.left = rect_table.get_or("left", 0.f);
				rect.right = rect_table.get_or("right", 0.f);
				rect.top = rect_table.get_or("top", 0.f);
				rect.bottom = rect_table.get_or("bottom", 0.f);

				element.set_rect(rect);
			};

			usertype["registereventhandler"] = [&state](ui_element& element, const std::string& name, const sol::protected_function& function)
			{
				element.register_event_handler(name, [&state, function](ui_element& element, const event_t& event)
				{
					auto event_table = state.create_table();

					event_table["name"] = event.name;
					for (auto& [k, v] : event)
					{
						event_table[k] = convert(state.lua_state(), v);
					}

					function(element, event_table);
				});
			};

			return usertype;
		}

		auto register_ui_element(sol::state& state)
		{
			auto usertype = state.new_usertype<ui_element>("uielement");

			register_base_methods(state, usertype);

			state["lui"]["uielement"] = state.create_table();
			state["lui"]["uielement"]["create"] = []()
			{
				return ui_element::create();
			};

			return usertype;
		}

		auto register_ui_image(sol::state& state)
		{
			auto usertype = state.new_usertype<ui_image>("uiimage", sol::base_classes, sol::bases<ui_element>());

			register_base_methods(state, usertype);

			state["lui"]["uiimage"] = state.create_table();
			state["lui"]["uiimage"]["create"] = []()
			{
				return ui_image::create();
			};

			return usertype;
		}

		auto register_ui_text(sol::state& state)
		{
			auto usertype = state.new_usertype<ui_text>("uitext", sol::base_classes, sol::bases<ui_element>());

			register_base_methods(state, usertype);

			state["lui"]["uitext"] = state.create_table();
			state["lui"]["uitext"]["create"] = []()
			{
				return ui_text::create();
			};

			usertype["settext"] = &ui_text::set_text;
			usertype["setformatted"] = &ui_text::set_formatted;
			usertype["setusestencil"] = &ui_text::set_use_stencil;
			usertype["setusewordwrapping"] = &ui_text::set_use_word_wrapping;
			usertype["setoutlinecolor"] = &ui_text::set_outline_color;

			return usertype;
		}

		auto register_ui_timer(sol::state& state)
		{
			auto usertype = state.new_usertype<ui_timer>("uitimer", sol::base_classes, sol::bases<ui_element>());

			register_base_methods(state, usertype);

			state["lui"]["uitimer"] = state.create_table();
			state["lui"]["uitimer"]["create"] = sol::overload(
				[](const std::string& event, const std::uint32_t delay)
				{
					return ui_timer::create(event, delay);
				},
				[](const std::string& event, const std::uint32_t delay, const bool looping)
				{
					return ui_timer::create(event, delay, looping);
				}
			);

			usertype["setdelay"] = &ui_timer::set_delay;
			usertype["setlooping"] = &ui_timer::set_looping;

			return usertype;
		}

		auto register_ui_button(sol::state& state)
		{
			auto usertype = state.new_usertype<ui_button>("uibutton", sol::base_classes, sol::bases<ui_element>());

			register_base_methods(state, usertype);

			state["lui"]["uibutton"] = state.create_table();
			state["lui"]["uibutton"]["create"] = [](ui_button&, const sol::table& properties_table)
			{
				button_properties_t properties{};

				properties.text = properties_table.get_or("text", std::string{});
				properties.style = properties_table.get_or("style", BUTTON_STYLE_DEFAULT);

				auto action = properties_table["action"];
				if (action.is<sol::protected_function>())
				{
					auto action_cb = action.get<sol::protected_function>();
					properties.action = [action_cb]()
					{
						action_cb();
					};
				}
				else if (action.is<std::string>())
				{
					properties.action = action.get<std::string>();
				}

				return ui_button::create(properties);
			};

			return usertype;
		}

		auto register_ui_list(sol::state& state)
		{
			auto usertype = state.new_usertype<ui_list>("uilist", sol::base_classes, sol::bases<ui_element>());

			register_base_methods(state, usertype);

			state["lui"]["uilist"] = state.create_table();
			state["lui"]["uilist"]["create"] = [](const float spacing)
			{
				return ui_list::create(spacing);
			};

			usertype["addchild"] = [](ui_list& element, ui_element& child)
			{
				element.add_child(child.shared_from_this());
			};

			usertype["removechild"] = [](ui_list& element, ui_element& child)
			{
				element.remove_child(child.shared_from_this());
			};

			usertype["removeallchildren"] = [](ui_list& element)
			{
				element.remove_all_children();
			};

			return usertype;
		}

		auto register_ui_menu(sol::state& state)
		{
			auto usertype = state.new_usertype<ui_menu>("uimenu", sol::base_classes, sol::bases<ui_element>());

			register_base_methods(state, usertype);

			state["lui"]["uimenu"] = state.create_table();
			state["lui"]["uimenu"]["create"] = [](const std::string& name)
			{
				return ui_menu::create(name);
			};

			usertype["addbackbutton"] = &ui_menu::add_back_button;
			usertype["adddescription"] = &ui_menu::add_description;
			usertype["addbutton"] = sol::overload(
				[](ui_menu& element, const std::string& text, const sol::lua_value& action)
				{
					if (action.is<std::string>())
					{
						element.add_button(text, action.as<std::string>());
					}
					else if (action.is<sol::protected_function>())
					{
						auto action_cb = action.as<sol::protected_function>();
						element.add_button(text, [action_cb]()
						{
							action_cb();
						});
					}
					else
					{
						element.add_button(text, {});
					}
				},
				[](ui_menu& element, const std::string& text, const sol::lua_value& action, const std::string& description)
				{
					if (action.is<std::string>())
					{
						element.add_button(text, action.as<std::string>(), description);
					}
					else if (action.is<sol::protected_function>())
					{
						element.add_button(text, [=]()
						{
							action.as<sol::protected_function>();
						}, description);
					}
					else
					{
						element.add_button(text, {}, description);
					}
				}
			);

			return usertype;
		}

		void register_enums(sol::state& state)
		{
			state["lui"]["ANIMATE_NORMAL"] = ANIMATE_NORMAL;
			state["lui"]["ANIMATE_REPEAT"] = ANIMATE_REPEAT;
			state["lui"]["ANIMATE_LOOP"] = ANIMATE_LOOP;
		}

		void register_structs(sol::state& state)
		{

		}

		void initialize_state(sol::state& state)
		{
			state.open_libraries
			(
				sol::lib::base,
				sol::lib::string,
				sol::lib::math,
				sol::lib::table
			);

			state["lui"] = sol::table::create(state.lua_state());

			register_ui_element(state);
			register_ui_image(state);
			register_ui_text(state);
			register_ui_timer(state);
			register_ui_button(state);
			register_ui_list(state);
			register_ui_menu(state);

			state["lui"]["flowmanager"] = sol::table::create(state.lua_state());
			state["lui"]["getroot"] = get_root_element;
			state["lui"]["flowmanager"]["requestmenu"] = flow_manager::request_menu;
			state["lui"]["flowmanager"]["requestpopmenu"] = flow_manager::request_pop_menu;
			state["lui"]["flowmanager"]["requestpopallmenus"] = flow_manager::request_pop_all_menus;
			state["lui"]["flowmanager"]["registermenu"] = [](const std::string& name, const sol::protected_function& callback)
			{
				flow_manager::register_menu(name, [=]()
				{
					const auto result = callback();
					auto element = result.get<sol::lua_value>(0);

#define TRY_TYPE(__type__) \
					if (element.is<__type__>()) \
					{ \
						return element.as<__type__>().shared_from_this(); \
					} \

					TRY_TYPE(ui_element);
					TRY_TYPE(ui_text);
					TRY_TYPE(ui_button);
					TRY_TYPE(ui_list);
					TRY_TYPE(ui_timer);
					TRY_TYPE(ui_image);
					TRY_TYPE(ui_menu);

					throw std::runtime_error("not a valid menu element");
				});
			};
		}
	}

	void start()
	{
		scripting_state = {};
		initialize_state(scripting_state);
		load_scripts("tpp-mod/ui_scripts/");
	}

	void run_frame()
	{
		scripting_state.collect_garbage();
	}

	void stop()
	{
		scripting_state = {};
	}
}
