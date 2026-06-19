#include <std_include.hpp>

#include "main.hpp"
#include "scripting.hpp"
#include "flow_manager.hpp"
#include "utils.hpp"

#include "../scripting.hpp"
#include "../command.hpp"
#include "../vars.hpp"
#include "../fobs.hpp"

#include "types/ui_button.hpp"
#include "types/ui_element.hpp"
#include "types/ui_image.hpp"
#include "types/ui_list.hpp"
#include "types/ui_menu.hpp"
#include "types/ui_text.hpp"
#include "types/ui_text_input.hpp"
#include "types/ui_timer.hpp"

#include "lua_types/json.hpp"

#include <utils/io.hpp>

namespace lui::scripting
{
	namespace
	{
		sol::state& get_state()
		{
			static sol::state state;
			return state;
		}

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

		void load_scripts(sol::state& state, const std::string& script_dir)
		{
			if (!::utils::io::directory_exists(script_dir))
			{
				return;
			}

			const auto scripts = ::utils::io::list_files(script_dir);

			for (const auto& script : scripts)
			{
				if (!script.ends_with(".lua"))
				{
					continue;
				}

				try
				{
					state.safe_script_file(script);
				}
				catch (const std::exception& e)
				{
					console::error("LUI: error loading script \"%s\": %s\n", script.data(), e.what());
				}
			}
		}

		sol::lua_value cast_element(sol::state& state, ui_element& element)
		{
			switch (element.get_type())
			{
			case UI_TEXT:
				return {state, dynamic_cast<ui_text*>(&element)};
			case UI_TEXT_INPUT:
				return {state, dynamic_cast<ui_text_input*>(&element)};
			case UI_IMAGE:
				return {state, dynamic_cast<ui_image*>(&element)};
			case UI_BUTTON:
				return {state, dynamic_cast<ui_button*>(&element)};
			case UI_LIST:
				return {state, dynamic_cast<ui_list*>(&element)};
			case UI_MENU:
				return {state, dynamic_cast<ui_menu*>(&element)};
			case UI_TIMER:
				return {state, dynamic_cast<ui_timer*>(&element)};
			};

			return {state, &element};
		}

		template <typename T>
		auto register_base_methods(sol::state& state, T usertype)
		{
			usertype["getid"] = &ui_element::get_id;
			usertype["setid"] = &ui_element::set_id;

			usertype[sol::meta_function::index] = [](const ui_element& element, const sol::this_state s,
				const sol::lua_value& key)
			{
				return element.lua_metadata[key];
			};

			usertype[sol::meta_function::new_index] = [](ui_element& element, const sol::this_state s,
				const sol::lua_value& key, const sol::lua_value& value)
			{
				element.lua_metadata[key] = value;
			};

			usertype["removeallchildren"] = &ui_element::remove_all_children;
			usertype["close"] = [](ui_element& element)
			{
				element.close();
			};

			usertype["getfirstchild"] = &ui_element::get_first_child;
			usertype["getlastchild"] = &ui_element::get_last_child;
			usertype["getallchildren"] = &ui_element::get_all_children;
			usertype["getfirstdescendantbyid"] = &ui_element::get_first_descendant_by_id;

			usertype["sethandlemouse"] = &ui_element::set_handle_mouse;
			usertype["sethandlemousemove"] = &ui_element::set_handle_mouse_move;
			usertype["sethandlekeys"] = &ui_element::set_handle_keys;
			usertype["setmouseblocking"] = &ui_element::set_mouse_blocking;
			usertype["setneedskeycatcher"] = &ui_element::set_needs_key_catcher;
			usertype["ismousein"] = &ui_element::is_mouse_in;
			usertype["makedraggable"] = &ui_element::make_draggable;

			usertype["setcolor"] = &ui_element::set_color;

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
				element_state_t current_state{};
				auto has_this_state = false;
				element.get_animation_state(name, current_state, has_this_state);
				if (!has_this_state)
				{
					element.get_animation_state("default", current_state, has_this_state);
				}

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

				element_state_t element_state{};
				element_state.position.anchor = anchor;
				element_state.position.rect.top = state.get_or("top", current_state.position.rect.top);
				element_state.position.rect.left = state.get_or("left", current_state.position.rect.left);
				element_state.position.rect.bottom = state.get_or("bottom", current_state.position.rect.bottom);
				element_state.position.rect.right = state.get_or("right", current_state.position.rect.right);
				element_state.width = state.get_or("width", current_state.width);
				element_state.height = state.get_or("height", current_state.height);
				element_state.position.rotation = state.get_or("rotation", current_state.position.rotation);
				element_state.position.alignment = state.get_or("alignment", current_state.position.alignment);
				element_state.position.vertical_alignment = state.get_or("verticalalignment", current_state.position.vertical_alignment);

				auto color = state["color"];
				if (color.is<sol::table>())
				{
					element_state.color.r = color["r"].get_or(current_state.color.r);
					element_state.color.g = color["g"].get_or(current_state.color.g);
					element_state.color.b = color["b"].get_or(current_state.color.b);
					element_state.color.a = color["a"].get_or(current_state.color.a);
				}
				else
				{
					element_state.color.r = current_state.color.r;
					element_state.color.g = current_state.color.g;
					element_state.color.b = current_state.color.b;
					element_state.color.a = current_state.color.a;
				}

				auto perspective = state["perspective"];
				if (perspective.is<sol::table>())
				{
					element_state.position.perspective.params[0] = perspective["param0"].get_or(current_state.position.perspective.params[0]);
					element_state.position.perspective.params[1] = perspective["param1"].get_or(current_state.position.perspective.params[1]);
					element_state.position.perspective.params[2] = perspective["param2"].get_or(current_state.position.perspective.params[2]);
					element_state.position.perspective.params[3] = perspective["param3"].get_or(current_state.position.perspective.params[3]);
					element_state.position.perspective.params[4] = perspective["param4"].get_or(current_state.position.perspective.params[4]);
				}
				else
				{
					element_state.position.perspective.params[0] = current_state.position.perspective.params[0];
					element_state.position.perspective.params[1] = current_state.position.perspective.params[1];
					element_state.position.perspective.params[2] = current_state.position.perspective.params[2];
					element_state.position.perspective.params[3] = current_state.position.perspective.params[3];
					element_state.position.perspective.params[4] = current_state.position.perspective.params[4];
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

			usertype["animateinsequence"] = [](ui_element& element, const sol::table& lua_entries)
			{
				std::vector<animation_sequence_t::entry_t> entries;

				for (const auto& [k, v] : lua_entries)
				{
					if (!v.is<sol::table>())
					{
						continue;
					}

					auto lua_entry = v.as<sol::table>();
					animation_sequence_t::entry_t entry{};
					auto state = lua_entry[1];
					auto duration = lua_entry[2];

					if (!state.is<std::string>() || !duration.is<std::int32_t>())
					{
						continue;
					}

					entry.state = state.get<std::string>();
					entry.duration = duration.get<std::int32_t>();
					entries.emplace_back(entry);
				}

				element.animate_in_sequence(entries);
			};

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

			usertype["registereventhandler"] = [&state](ui_element& element, const std::string& name, const sol::unsafe_function& function)
			{
				element.register_event_handler(name, [&state, function](ui_element& element, const event_t& event)
				{
					function(cast_element(state, element), event);
				});
			};

			usertype["dispatchevent"] = sol::overload(
				[](ui_element& element, const event_t& event)
				{
					element.dispatch_event(event);
				},
				[](ui_element& element, const sol::table& event_table)
				{
					event_t event{};
					event.target = element.shared_from_this();
					auto has_name = false;

					for (auto& [k, v] : event_table)
					{
						if (!k.is<std::string>())
						{
							continue;
						}

						const auto key_value = k.as<std::string>();

						if (key_value == "name")
						{
							event.name = v.as<std::string>();
							has_name = true;
						}
						else if (key_value == "dispatchchildren")
						{
							event.dispatch_children = v.as<bool>();
						}
						else if (key_value == "immediate")
						{
							event.immediate = v.as<bool>();
						}
						else
						{
							const auto converted = convert(v);
							event.params.set(key_value, converted);
						}
					}

					if (!has_name)
					{
						throw std::runtime_error("event must have a name");
					}

					element.dispatch_event(event);
				}
			);

			return usertype;
		}

		auto register_ui_element(sol::state& state)
		{
			auto usertype = state.new_usertype<ui_element>("uielement");

			register_base_methods(state, usertype);

			usertype["cast"] = [&state](ui_element& element)
			{
				return cast_element(state, element);
			};

			state["lui"]["uielement"] = state.create_table();
			state["lui"]["uielement"]["new"] = [&state]()
			{
				auto element = ui_element::create();
				element->lua_metadata = state.create_table();
				return element;
			};

			return usertype;
		}

		auto register_ui_image(sol::state& state)
		{
			auto usertype = state.new_usertype<ui_image>("uiimage", sol::base_classes, sol::bases<ui_element>());

			register_base_methods(state, usertype);

			state["lui"]["uiimage"] = state.create_table();
			state["lui"]["uiimage"]["new"] = [&state]()
			{
				auto element = ui_image::create();
				element->lua_metadata = state.create_table();
				return element;
			};

			usertype["setuvcenter"] = sol::overload(
				[](ui_image& element, const float u, const float v, const std::uint32_t type)
				{
					element.set_uv_center(u, v, type);
				},
				[](ui_image& element, const float u, const float v)
				{
					element.set_uv_center(u, v, TEXTURE_BASE);
					element.set_uv_center(u, v, TEXTURE_SCREEN);
					element.set_uv_repeat(u, v, TEXTURE_MASK);
					element.set_uv_center(u, v, TEXTURE_LAYER);
				}
			);
			
			usertype["setblend"] = sol::overload(
				[](ui_image& element, const float blend, const std::uint32_t type)
				{
					element.set_blend(blend, type);
				},
				[](ui_image& element, const float blend)
				{
					element.set_blend(blend, TEXTURE_BASE);
					element.set_blend(blend, TEXTURE_SCREEN);
					element.set_blend(blend, TEXTURE_MASK);
					element.set_blend(blend, TEXTURE_LAYER);
				}
			);

			usertype["setuvshift"] = sol::overload(
				[](ui_image& element, const float u, const float v, const std::uint32_t type)
				{
					element.set_uv_shift(u, v, type);
				},
				[](ui_image& element, const float u, const float v)
				{
					element.set_uv_shift(u, v, TEXTURE_BASE);
					element.set_uv_shift(u, v, TEXTURE_SCREEN);
					element.set_uv_shift(u, v, TEXTURE_MASK);
					element.set_uv_shift(u, v, TEXTURE_LAYER);
				}
			);

			usertype["setuvrepeat"] = sol::overload(
				[](ui_image& element, const float u, const float v, const std::uint32_t type)
				{
					element.set_uv_repeat(u, v, type);
				},
				[](ui_image& element, const float u, const float v)
				{
					element.set_uv_repeat(u, v, TEXTURE_BASE);
					element.set_uv_repeat(u, v, TEXTURE_SCREEN);
					element.set_uv_repeat(u, v, TEXTURE_MASK);
					element.set_uv_repeat(u, v, TEXTURE_LAYER);
				}
			);
			
			usertype["setshader"] = sol::overload(
				[](ui_image& element, const std::uint64_t hash)
				{
					element.set_shader(hash);
				},
				[](ui_image& element, const std::string& path)
				{
					element.set_shader(path);
				}
			);

			usertype["setbasetexture"] = sol::overload(
				[](ui_image& element, const std::uint64_t hash)
				{
					element.set_texture(hash, TEXTURE_BASE);
				},
				[](ui_image& element, const std::string& path)
				{
					element.set_texture(path, TEXTURE_BASE);
				}
			);
			
			usertype["setscreentexture"] = sol::overload(
				[](ui_image& element, const std::uint64_t hash)
				{
					element.set_texture(hash, TEXTURE_SCREEN);
				},
				[](ui_image& element, const std::string& path)
				{
					element.set_texture(path, TEXTURE_SCREEN);
				}
			);
						
			usertype["setmasktexture"] = sol::overload(
				[](ui_image& element, const std::uint64_t hash)
				{
					element.set_texture(hash, TEXTURE_MASK);
				},
				[](ui_image& element, const std::string& path)
				{
					element.set_texture(path, TEXTURE_MASK);
				}
			);
						
			usertype["setlayertexture"] = sol::overload(
				[](ui_image& element, const std::uint64_t hash)
				{
					element.set_texture(hash, TEXTURE_LAYER);
				},
				[](ui_image& element, const std::string& path)
				{
					element.set_texture(path, TEXTURE_LAYER);
				}
			);

			usertype["settexture"] = sol::overload(
				[](ui_image& element, const std::uint64_t hash, const std::uint32_t type)
				{
					element.set_texture(hash, type);
				},
				[](ui_image& element, const std::string& path, const std::uint32_t type)
				{
					element.set_texture(path, type);
				},
				[](ui_image& element, const std::uint64_t hash)
				{
					element.set_texture(hash);
				},
				[](ui_image& element, const std::string& path)
				{
					element.set_texture(path);
				}
			);

			usertype["setrandomuvshift"] = &ui_image::set_random_uv_shift;

			return usertype;
		}

		auto register_ui_text(sol::state& state)
		{
			auto usertype = state.new_usertype<ui_text>("uitext", sol::base_classes, sol::bases<ui_element>());

			register_base_methods(state, usertype);

			state["lui"]["uitext"] = state.create_table();
			state["lui"]["uitext"]["new"] = [&state]()
			{
				auto element = ui_text::create();
				element->lua_metadata = state.create_table();
				return element;
			};

			usertype["settext"] = &ui_text::set_text;
			usertype["setformatted"] = &ui_text::set_formatted;
			usertype["setusestencil"] = &ui_text::set_use_stencil;
			usertype["setusewordwrapping"] = &ui_text::set_use_word_wrapping;
			usertype["setoutlinecolor"] = &ui_text::set_outline_color;
			usertype["setfont"] = &ui_text::set_font;

			return usertype;
		}

		auto register_ui_input_text(sol::state& state)
		{
			auto usertype = state.new_usertype<ui_text_input>("uitextinput", sol::base_classes, sol::bases<ui_element>());

			register_base_methods(state, usertype);

			state["lui"]["uitextinput"] = state.create_table();
			state["lui"]["uitextinput"]["new"] = [&state]()
			{
				auto element = ui_text_input::create();
				element->lua_metadata = state.create_table();
				return element;
			};

			usertype["setinput"] = &ui_text_input::set_input;
			usertype["sethinttext"] = &ui_text_input::set_hint_text;
			usertype["setcursor"] = &ui_text_input::set_cursor;
			usertype["setfocused"] = &ui_text_input::set_focused;
			usertype["setformatted"] = &ui_text_input::set_formatted;
			usertype["clear"] = &ui_text_input::clear;

			return usertype;
		}

		auto register_ui_timer(sol::state& state)
		{
			auto usertype = state.new_usertype<ui_timer>("uitimer", sol::base_classes, sol::bases<ui_element>());

			register_base_methods(state, usertype);

			state["lui"]["uitimer"] = state.create_table();
			state["lui"]["uitimer"]["new"] = sol::overload(
				[&state](const std::string& event, const std::uint32_t delay)
				{
					auto element = ui_timer::create(event, delay);
					element->lua_metadata = state.create_table();
					return element;
				},
				[&](const std::string& event, const std::uint32_t delay, const bool looping)
				{
					auto element = ui_timer::create(event, delay, looping);
					element->lua_metadata = state.create_table();
					return element;
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
			state["lui"]["uibutton"]["new"] = [&state](ui_button&, const sol::table& properties_table)
			{
				button_properties_t properties{};

				properties.text = properties_table.get_or("text", std::string{});
				properties.style = properties_table.get_or("style", BUTTON_STYLE_DEFAULT);

				auto action = properties_table["action"];
				if (action.is<sol::unsafe_function>())
				{
					auto action_cb = action.get<sol::unsafe_function>();
					properties.action = [action_cb]()
					{
						action_cb();
					};
				}
				else if (action.is<std::string>())
				{
					properties.action = action.get<std::string>();
				}

				auto element = ui_button::create(properties);
				element->lua_metadata = state.create_table();
				return element;
			};

			return usertype;
		}

		auto register_ui_list(sol::state& state)
		{
			auto usertype = state.new_usertype<ui_list>("uilist", sol::base_classes, sol::bases<ui_element>());

			register_base_methods(state, usertype);

			state["lui"]["uilist"] = state.create_table();
			state["lui"]["uilist"]["new"] = [&state](const float spacing)
			{
				auto element = ui_list::create(spacing);
				element->lua_metadata = state.create_table();
				return element; 
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
			state["lui"]["uimenu"]["new"] = [&state](const std::string& name)
			{
				auto element = ui_menu::create(name);
				element->lua_metadata = state.create_table();
				return element;
			};

			usertype["addbackbutton"] = &ui_menu::add_back_button;
			usertype["adddescription"] = &ui_menu::add_description;
			usertype["addbutton"] = sol::overload(
				[](ui_menu& element, const std::string& text, const sol::lua_value& action)
				{
					if (action.is<std::string>())
					{
						return element.add_button(text, action.as<std::string>());
					}
					else if (action.is<sol::unsafe_function>())
					{
						auto action_cb = action.as<sol::unsafe_function>();
						return element.add_button(text, [action_cb]()
						{
							action_cb();
						});
					}
					else
					{
						return element.add_button(text, {});
					}
				},
				[](ui_menu& element, const std::string& text, const sol::lua_value& action, const std::string& description)
				{
					if (action.is<std::string>())
					{
						return element.add_button(text, action.as<std::string>(), description);
					}
					else if (action.is<sol::unsafe_function>())
					{
						return element.add_button(text, [=]()
						{
							action.as<sol::unsafe_function>();
						}, description);
					}
					else
					{
						return element.add_button(text, {}, description);
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

			state["lui"]["BUTTON_STYLE_DEFAULT"] = BUTTON_STYLE_DEFAULT;

			state["lui"]["FONT_SYSTEM"] = FONT_SYSTEM;
			state["lui"]["FONT_ARTIST"] = FONT_ARTIST;

			state["lui"]["TEXTURE_BASE"] = TEXTURE_BASE;
			state["lui"]["TEXTURE_SCREEN"] = TEXTURE_SCREEN;
			state["lui"]["TEXTURE_MASK"] = TEXTURE_MASK;
			state["lui"]["TEXTURE_LAYER"] = TEXTURE_LAYER;

			state["lui"]["ALIGN_LEFT"] = ALIGN_LEFT;
			state["lui"]["ALIGN_CENTER"] = ALIGN_CENTER;
			state["lui"]["ALIGN_RIGHT"] = ALIGN_RIGHT;
			state["lui"]["ALIGN_TOP"] = ALIGN_TOP;
			state["lui"]["ALIGN_MIDDLE"] = ALIGN_MIDDLE;
			state["lui"]["ALIGN_BOTTOM"] = ALIGN_BOTTOM;
		}

		void register_structs(sol::state& state)
		{
			auto object_usertype = state.new_usertype<object>("object_value");

			object_usertype[sol::meta_function::index] = [&state](const object& object, const std::string& key)
			{
				const auto& values = object.get_values();
				const auto iter = values.find(key);
				if (iter == values.end())
				{
					return sol::lua_value{state, sol::lua_nil};
				}

				return convert(state.lua_state(), iter->second);
			};

			object_usertype[sol::meta_function::new_index] = [&state](object& object, const std::string& key, const sol::lua_value& value)
			{
				object.set(key, convert(value));
			};

			object_usertype["new"] = []()
			{
				return object();
			};

			auto event_usertype = state.new_usertype<event_t>("uievent", 
				"name", &event_t::name, 
				"immediate", &event_t::immediate,
				"dispatchchildren", &event_t::dispatch_children,
				"params", &event_t::params
			);

			event_usertype["new"] = []()
			{
				return event_t();
			};
		}

		void register_utility(sol::state& state)
		{
			setup_json(state);

			state["game"] = sol::table::create(state.lua_state());

			state["game"]["playsound"] = utils::play_sound;
			state["game"]["executelua"] = [](const std::string& code)
				-> sol::lua_value
			{
				const auto res = ::scripting::script_exec(code);
				if (res.has_value())
				{
					switch (res->get_type())
					{
					case LUA_TBOOLEAN:
						return res->get_bool();
					case LUA_TNUMBER:
						return res->get_number();
					case LUA_TSTRING:
						return res->get_string();
					}
				}

				return sol::lua_nil;
			};

			state["game"]["localize"] = [](const std::string& str)
			{
				game::fox::StringId str_id{};
				game::tpp::ui::utility::GetStringId(&str_id, str.data());
				return game::tpp::ui::utility::GetLangText(str_id);
			};

			state["game"]["getcurrentlocationid"] = []()
			{
				return game::tpp::ui::utility::GetCurrentLocationId();
			};

			state["game"]["getcurrentmissionid"] = []()
			{
				return game::tpp::ui::utility::GetCurrentMissionId();
			};

			state["game"]["getmaterialresource"] = sol::overload(
				[](const std::string& path)
				{
					return utils::get_material_resource(path);
				},
				[](const std::uint64_t hash)
				{
					return utils::get_material_resource(hash);
				}
			);

			state["game"]["gettextureresource"] = sol::overload(
				[](const std::string& path)
				{
					return utils::get_texture_resource(path);
				},
				[](const std::uint64_t hash)
				{
					return utils::get_texture_resource(hash);
				}
			);

			state["utils"] = sol::table::create(state.lua_state());
			state["utils"]["executecommand"] = [](const std::string& cmd)
			{
				command::execute(cmd);
			};

			state["utils"]["getvarstring"] = [](const std::string& name)
			{
				const auto var = vars::find(name);
				if (var == nullptr)
				{
					return std::string{};
				}

				return var->current.to_string();
			};

			state["utils"]["getvarint"] = [](const std::string& name)
			{
				const auto var = vars::find(name);
				if (var == nullptr || var->type != vars::var_type_integer)
				{
					return 0;
				}

				return var->current.get_int();
			};

			state["utils"]["getvarfloat"] = [](const std::string& name)
			{
				const auto var = vars::find(name);
				if (var == nullptr || var->type != vars::var_type_float)
				{
					return 0.f;
				}

				return var->current.get_float();
			};

			state["utils"]["setvarfromstring"] = vars::set_var_from_string;

			state["fobs"] = sol::table::create(state.lua_state());
			state["fobs"]["addcustomtarget"] = [](const std::string& type, const sol::table& target)
			{
				static game::tpp::mbm::PlayerBasicInfo info;
				std::memset(&info, 0, sizeof(info));

				auto owner_info_v = target.get<sol::lua_value>("owner_info");
				auto mother_base_param_v = target.get<sol::lua_value>("mother_base_param");
				auto owner_detail_record_v = target.get<sol::lua_value>("owner_detail_record");
				auto owner_fob_record_v = target.get<sol::lua_value>("owner_fob_record");
				
				if (owner_info_v.is<sol::table>())
				{
					auto owner_info = owner_info_v.as<sol::table>();
					info.owner_account.id = owner_info.get_or("xuid", 0ull);
					info.owner_player_id = owner_info.get_or("player_id", 0);
					info.cluster = static_cast<char>(owner_info.get_or("cluster", 0));
					info.owner_ugc = owner_info.get_or("ugc", 1) == 1;
				}

				if (mother_base_param_v.is<sol::table>())
				{
					auto mother_base_param = mother_base_param_v.as<sol::table>();

					for (auto& [k, v] : mother_base_param)
					{
						if (info.mother_base_num >= 4)
						{
							break;
						}

						auto param = v.as<sol::table>();
						auto i = info.mother_base_num++;
						const auto construct_param = param.get_or("construct_param", 0);

						info.mother_base_id[i] = param.get_or("mother_base_id", 0);
						info.platform_count[i] = static_cast<char>(param.get_or("platform_count", 0));
						info.security_rank[i] = static_cast<char>(param.get_or("security_rank", 0));
						info.area_id[i] = static_cast<char>((construct_param >> 1) & 0x7F);
						info.construct_param2[i] = static_cast<char>((construct_param >> 22) & 0x3F);
					}
				}

				info.mother_base_num++;

				if (owner_detail_record_v.is<sol::table>())
				{
					auto owner_detail_record = owner_detail_record_v.as<sol::table>();
					
					auto emblem_v = owner_detail_record.get<sol::lua_value>("emblem");
					auto espionage_v = owner_detail_record.get<sol::lua_value>("espionage");
					auto sneak_rank_v = owner_detail_record.get<sol::lua_value>("sneak_rank");
					auto league_rank_v = owner_detail_record.get<sol::lua_value>("league_rank");

					info.fields1.follow = owner_detail_record.get_or("follow", 0) == 1;
					info.fields1.follower = owner_detail_record.get_or("follower", 0) == 1;
					info.fields1.enemy = owner_detail_record.get_or("enemy", 0) == 1;
					info.fields1.help = owner_detail_record.get_or("help", 0) == 1;
					info.fields1.online = owner_detail_record.get_or("online", 0) == 1;
					info.fields1.insurance = owner_detail_record.get_or("insurance", 0) == 1;
					info.fields1.hero = owner_detail_record.get_or("hero", 0) == 1;
					info.nameplate_id = static_cast<char>(owner_detail_record.get_or("name_plate_id", 0));
					info.nuclear = static_cast<char>(owner_detail_record.get_or("nuclear", 0));
					info.staff_num = static_cast<short>(owner_detail_record.get_or("staff_count", 0));

					if (espionage_v.is<sol::table>())
					{
						auto espionage = espionage_v.as<sol::table>();
						info.espionage_section = static_cast<short>(espionage.get_or("section", 0));
						info.espionage_win = static_cast<short>(espionage.get_or("win", 0));
						info.espionage_score = espionage.get_or("score", 0);
						info.espionage_total = info.espionage_win + static_cast<short>(espionage.get_or("lose", 0));
					}

					if (league_rank_v.is<sol::table>())
					{
						auto league_rank = league_rank_v.as<sol::table>();
						info.league_rank_grade = static_cast<char>(league_rank.get_or("grade", 0));
						info.league_rank_rank = league_rank.get_or("rank", 0);
					}

					if (sneak_rank_v.is<sol::table>())
					{
						auto sneak_rank = sneak_rank_v.as<sol::table>();
						info.sneak_rank_grade = static_cast<char>(sneak_rank.get_or("grade", 0));
						info.sneak_rank_rank = sneak_rank.get_or("rank", 0);
					}

					if (emblem_v.is<sol::table>())
					{
						auto emblem = emblem_v.as<sol::table>();
						auto parts = emblem.get<sol::table>("parts");

						auto i = 0;
						for (auto& [k, v] : parts)
						{
							auto part = v.as<sol::table>();
							info.owner_emblem.texture_tag[i] = part.get_or("texture_tag", 0);
							info.owner_emblem.base_color[i] = part.get_or("base_color", 0);
							info.owner_emblem.frame_color[i] = part.get_or("frame_color", 0);
							info.owner_emblem.position_x[i] = static_cast<char>(part.get_or("position_x", 0));
							info.owner_emblem.position_y[i] = static_cast<char>(part.get_or("position_y", 0));
							info.owner_emblem.scale[i] = static_cast<char>(part.get_or("scale", 0));
							info.owner_emblem.rotate[i] = static_cast<char>(part.get_or("rotate", 0));

							i++;
							if (i >= 4)
							{
								break;
							}
						}
					}
				}
			
				if (owner_fob_record_v.is<sol::table>())
				{
					auto owner_fob_record = owner_fob_record_v.as<sol::table>();

					auto processing_resource_v = owner_fob_record.get<sol::lua_value>("processing_resource");
					auto usable_resource_v = owner_fob_record.get<sol::lua_value>("usable_resource");
					auto capture_resource_v = owner_fob_record.get<sol::lua_value>("capture_resource");
					auto staff_count_v = owner_fob_record.get<sol::lua_value>("staff_count");
					auto capture_staff_count_v = owner_fob_record.get<sol::lua_value>("capture_staff_count");
					auto injury_staff_count_v = owner_fob_record.get<sol::lua_value>("injury_staff_count");
				
					info.attack_count = owner_fob_record.get_or("attack_count", 0);
					info.attack_gmp = owner_fob_record.get_or("attack_gmp", 0);
					info.capture_nuclear = owner_fob_record.get_or("capture_nuclear", 0);
					info.capture_resource_count = owner_fob_record.get_or("capture_resource_count", 0);
					info.capture_staff = owner_fob_record.get_or("capture_staff", 0);
					info.date_time = owner_fob_record.get_or("date_time", 0);
					info.left_hour2 = owner_fob_record.get_or("left_hour", 0);
					info.support_count1 = owner_fob_record.get_or("support_count", 0);
					info.supported_count = owner_fob_record.get_or("supported_count", 0);
				
					if (processing_resource_v.is<sol::table>())
					{
						auto resource = processing_resource_v.as<sol::table>();
						info.processing_resource.fuel_resource = resource.get_or("fuel_resource", 0);
						info.processing_resource.biotic_resource = resource.get_or("biotic_resource", 0);
						info.processing_resource.common_metal = resource.get_or("common_metal", 0);
						info.processing_resource.minor_metal = resource.get_or("minor_metal", 0);
						info.processing_resource.precious_metal = resource.get_or("precious_metal", 0);
					}

					if (usable_resource_v.is<sol::table>())
					{
						auto resource = usable_resource_v.as<sol::table>();
						info.usable_resource.fuel_resource = resource.get_or("fuel_resource", 0);
						info.usable_resource.biotic_resource = resource.get_or("biotic_resource", 0);
						info.usable_resource.common_metal = resource.get_or("common_metal", 0);
						info.usable_resource.minor_metal = resource.get_or("minor_metal", 0);
						info.usable_resource.precious_metal = resource.get_or("precious_metal", 0);
					}

					if (capture_resource_v.is<sol::table>())
					{
						auto resource = capture_resource_v.as<sol::table>();
						info.capture_resource.fuel_resource = resource.get_or("fuel_resource", 0);
						info.capture_resource.biotic_resource = resource.get_or("biotic_resource", 0);
						info.capture_resource.common_metal = resource.get_or("common_metal", 0);
						info.capture_resource.minor_metal = resource.get_or("minor_metal", 0);
						info.capture_resource.precious_metal = resource.get_or("precious_metal", 0);
					}

					if (staff_count_v.is<sol::table>())
					{
						auto count = staff_count_v.as<sol::table>();
						auto i = 0;
						for (auto& [k, v] : count)
						{
							info.staff_count[i++] = v.as<short>();
							if (i >= 10)
							{
								break;
							}
						}
					}

					if (capture_staff_count_v.is<sol::table>())
					{
						auto count = capture_staff_count_v.as<sol::table>();
						auto i = 0;
						for (auto& [k, v] : count)
						{
							info.capture_staff_count[i++] = v.as<short>();
							if (i >= 10)
							{
								break;
							}
						}
					}

					if (injury_staff_count_v.is<sol::table>())
					{
						auto count = injury_staff_count_v.as<sol::table>();
						auto i = 0;
						for (auto& [k, v] : count)
						{
							info.injury_staff_count[i++] = v.as<short>();
							if (i >= 10)
							{
								break;
							}
						}
					}
				}

				fobs::add_custom_fob_target(type, info);
			};

			state["fobs"]["removecustomtarget"] = fobs::remove_custom_fob_target;
			state["fobs"]["clearcustomtargets"] = fobs::clear_custom_fob_targets;
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

			register_enums(state);
			register_utility(state);

			register_structs(state);

			register_ui_element(state);
			register_ui_image(state);
			register_ui_text(state);
			register_ui_input_text(state);
			register_ui_timer(state);
			register_ui_button(state);
			register_ui_list(state);
			register_ui_menu(state);

			state["lui"]["getroot"] = get_root_element;

			state["lui"]["flowmanager"] = sol::table::create(state.lua_state());
			state["lui"]["flowmanager"]["requestmenu"] = flow_manager::request_menu;
			state["lui"]["flowmanager"]["requestpopmenu"] = flow_manager::request_pop_menu;
			state["lui"]["flowmanager"]["requestpopallmenus"] = flow_manager::request_pop_all_menus;
			state["lui"]["flowmanager"]["registermenu"] = [](const std::string& name, const sol::unsafe_function& callback)
			{
				flow_manager::register_menu(name, [=]()
					-> ui_element_ptr
				{
					const auto result = callback();
					if (!result.valid() || result.return_count() == 0)
					{
						console::error("LUI: error opening menu \"%s\": not a valid ui element\n", name.data());
					}

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

					console::error("LUI: error opening menu \"%s\": not a valid ui element\n", name.data());
					return nullptr;
				});
			};
		}
	}

	void start()
	{
		auto& state = get_state();
		state = {};
		initialize_state(state);
		load_scripts(state, "tpp-mod/ui_scripts/");
	}

	void run_frame()
	{
		get_state().collect_garbage();
	}

	void stop()
	{
		get_state() = {};
	}
}
