#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "console.hpp"
#include "filesystem.hpp"
#include "vars.hpp"
#include "command.hpp"

#include <utils/io.hpp>
#include <utils/hook.hpp>
#include <utils/flags.hpp>
#include <utils/string.hpp>

namespace scripting
{
	namespace
	{
		utils::hook::detour lua_init_hook;
		utils::hook::detour lua_new_state_hook;

		bool loading_custom_script = false;

		vars::var_ptr var_lua_logging;
		vars::var_ptr var_lua_dump;

		void print_loading_script(const std::string& name)
		{
			console::info("Loading custom lua script '%s'\n", name.data());
		}

		void load_script(const std::string& path)
		{
			std::string data;
			if (!filesystem::read_file(path, &data))
			{
				return;
			}

			loading_custom_script = true;
			const auto _0 = gsl::finally([&]
			{
				loading_custom_script = false;
			});

			print_loading_script(path);

			const auto state = (*game::s_instances)->state;
			if (game::lua::luaL_loadbuffer(state, data.data(), data.size(), path.data()) != 0 ||
				game::lua::lua_pcall(state, 0, 0, 0) != 0)
			{
				size_t size{};
				const auto error = game::lua::lua_tolstring(state, -1, &size);
				console::error("Error loading script \"%s\": %s\n", path.data(), error);
				lua_pop(state, 1);
			}
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
				std::string data{};
				if (std::filesystem::is_directory(script))
				{
					load_script(script + "/__init__.lua");
				}
			}
		}

		void lua_init_stub(void* a1)
		{
			if (var_lua_logging->current.get_int() >= 1)
			{
				console::info("[lua]: initializing\n");
			}

			lua_init_hook.invoke<void>(a1);
			load_scripts("tpp-mod/scripts");

			if (game::environment::is_tpp())
			{
				load_scripts("tpp-mod/scripts/tpp");
			}
			else
			{
				load_scripts("tpp-mod/scripts/mgo");
			}
		}

		utils::hook::detour lual_load_buffer_hook;
		int lual_load_buffer_stub(lua_State* state, const char* buffer, size_t size, const char* name)
		{
			if (loading_custom_script)
			{
				return lual_load_buffer_hook.invoke<int>(state, buffer, size, name);
			}

			std::string name_ = name;
			if (name_.starts_with("@"))
			{
				name_ = name_.substr(1);
			}

			if (name_.starts_with("/"))
			{
				name_ = name_.substr(1);
			}

			if (var_lua_dump->current.enabled())
			{
				utils::io::write_file("tpp-mod/dump/lua/"s + name_, std::string(buffer, size));
			}

			std::string data;
			if (filesystem::read_file(name_, &data))
			{
				console::info("[lua] Overriding script \"%s\"", name);
				return lual_load_buffer_hook.invoke<int>(state, data.data(), data.size(), name);
			}
			else
			{
				if (var_lua_logging->current.get_int() >= 1)
				{
					console::info("[lua] Loading script \"%s\"", name);
				}

				return lual_load_buffer_hook.invoke<int>(state, buffer, size, name);
			}
		}

		template <console::console_type Type>
		void lua_print(lua_State* s)
		{
			if (var_lua_logging->current.get_int() < 2)
			{
				return;
			}

			size_t len{};
			const char* cstr = game::lua::lua_tolstring(s, -1, &len);
			std::string str(cstr, len);

			const char* type_name = "Log";
			if constexpr (Type == console::con_type_warning)
			{
				type_name = "Warning";
			}

			if constexpr (Type == console::con_type_error)
			{
				type_name = "Error";
			}

			console::print(Type, "[Fox.%s] %s\n", type_name, str.data());
		}

		std::string get_table_value(lua_State* state)
		{
			const auto type = game::lua::lua_type(state, -1);

			switch (type)
			{
			case LUA_TBOOLEAN:
				return utils::string::va("%i", game::lua::lua_toboolean(state, -1));
			case LUA_TNUMBER:
				return utils::string::va("%i", game::lua::lua_tointeger(state, -1));
			case LUA_TSTRING:
			{
				size_t len{};
				return game::lua::lua_tolstring(state, -1, &len);
			}
			default:
				return utils::string::va("(%s)", game::lua::lua_typename(state, type));
			}
		}

		void script_table_command_list_internal(lua_State* state, const std::string& name)
		{
			if (game::lua::lua_type(state, -1) == LUA_TTABLE)
			{
				game::lua::lua_pushnil(state);
				while (game::lua::lua_next(state, -2) != 0)
				{
					size_t len{};
					const char* key = game::lua::lua_tolstring(state, -2, &len);
					const auto value_str = get_table_value(state);
					console::info("%s.%s\n", name.data(), key, value_str.data());
					lua_pop(state, 1);
				}
			}
		}

		void script_get_recurse(lua_State* state, const std::string& str, bool stop_at_table = false, std::string* end_key = nullptr)
		{
			const auto keys = utils::string::split(str, '.');
			auto is_first = true;
			for (const auto& key : keys)
			{
				if (end_key != nullptr)
				{
					*end_key = key;
				}

				if (is_first)
				{
					game::lua::lua_getfield(state, LUA_GLOBALSINDEX, key.data());
					is_first = false;
				}
				else
				{
					game::lua::lua_getfield(state, -1, key.data());
					game::lua::lua_remove(state, -2);
				}

				if (game::lua::lua_type(state, -1) != LUA_TTABLE)
				{
					break;
				}
			}
		}

		void script_set_recurse(lua_State* state, const std::string& str, std::string* end_key)
		{
			const auto keys = utils::string::split(str, '.');
			auto is_first = true;
			for (const auto& key : keys)
			{
				const auto _0 = gsl::finally([&]
				{
					is_first = false;
				});

				if (is_first)
				{
					game::lua::lua_getfield(state, LUA_GLOBALSINDEX, key.data());
				}
				else
				{
					game::lua::lua_getfield(state, -1, key.data());
				}

				*end_key = key;

				if (game::lua::lua_type(state, -1) == LUA_TTABLE)
				{
					if (!is_first)
					{
						game::lua::lua_remove(state, -2);
					}
					continue;
				}
				else
				{
					lua_pop(state, 1);
					break;
				}
			}
		}

		void script_get_command(lua_State* state, const std::string& str)
		{
			script_get_recurse(state, str);

			if (game::lua::lua_type(state, -1) == LUA_TTABLE)
			{
				script_table_command_list_internal(state, str);
			}
			else
			{
				const auto type_name = game::lua::lua_typename(state, game::lua::lua_type(state, -1));
				const auto value_str = get_table_value(state);
				console::info("\"%s\" is \"%s\" type: %s\n", str.data(), value_str.data(), type_name);
			}

			lua_pop(state, 1);
		}

		void script_set_command(lua_State* state, const std::string& str, const std::string& value)
		{
			std::string key;
			script_set_recurse(state, str, &key);

			if (game::lua::lua_type(state, -1) == LUA_TTABLE)
			{
				game::lua::lua_getfield(state, -1, key.data());
				switch (game::lua::lua_type(state, -1))
				{
				case LUA_TBOOLEAN:
					game::lua::lua_pushboolean(state, std::atoi(value.data()) == 1);
					game::lua::lua_setfield(state, -3, key.data());
					break;
				case LUA_TNUMBER:
					game::lua::lua_pushnumber(state, std::atof(value.data()));
					game::lua::lua_setfield(state, -3, key.data());
					break;
				case LUA_TSTRING:
					game::lua::lua_pushstring(state, value.data());
					game::lua::lua_setfield(state, -3, key.data());
					break;
				}
			}

			lua_pop(state, 1);
		}

		void script_var_command(const command::params& params)
		{
			const auto state = (*game::s_instances)->state;
			if (state == nullptr)
			{
				return;
			}

			if (params.size() == 2)
			{
				script_get_command(state, params.get(1));
			}
			else if (params.size() >= 3)
			{
				script_set_command(state, params.get(1), params.get(2));
			}
		}

		void script_exec(const std::string& code)
		{
			const auto state = (*game::s_instances)->state;
			if (state == nullptr)
			{
				return;
			}

			loading_custom_script = true;
			const auto _0 = gsl::finally([&]
			{
				loading_custom_script = false;
			});

			if (game::lua::luaL_loadstring(state, code.data()) != 0 || game::lua::lua_pcall(state, 0, 0, 0) != 0)
			{
				size_t size{};
				const auto error = game::lua::lua_tolstring(state, -1, &size);
				console::error("Execution error: %s\n", error);
				lua_pop(state, 1);
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			var_lua_logging = vars::register_int("lua_logging", 0, 0, 2, vars::var_flag_saved, "enable lua logging (1: print scripts, 2: enable log prints)");
			var_lua_dump = vars::register_bool("lua_dump", false, vars::var_flag_saved, "dump lua scripts");
		}

		void start() override
		{
			utils::hook::inject(SELECT_VALUE(0x1435B49B6, 0x142EF23D6) + 3, lua_print<console::con_type_info>);
			utils::hook::inject(SELECT_VALUE(0x1435B49D9, 0x142EF23F9) + 3, lua_print<console::con_type_warning>);
			utils::hook::inject(SELECT_VALUE(0x1435B4A03, 0x142EF241C) + 3, lua_print<console::con_type_warning>);
			utils::hook::inject(SELECT_VALUE(0x1435B4A1F, 0x142EF243F) + 3, lua_print<console::con_type_error>);

			lua_init_hook.create(SELECT_VALUE(0x14319C450, 0x1425E5490), lua_init_stub);
			lual_load_buffer_hook.create(game::lua::luaL_loadbuffer, lual_load_buffer_stub);

			command::add("script_var", [](const command::params& params)
			{
				script_var_command(params);
			});

			command::add("script_exec", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				script_exec(params.join(1));
			});

			command::add("script_load", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				load_script(params.get(1));
			});

			if (game::environment::is_tpp())
			{
				command::add("disconnect", []
				{
					script_exec("TppMission.GameOverAbortMission()");
				});
			}
		}
	};
}

REGISTER_COMPONENT(scripting::component)
