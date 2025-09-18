#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "console.hpp"
#include "filesystem.hpp"
#include "vars.hpp"

#include <utils/io.hpp>
#include <utils/hook.hpp>
#include <utils/flags.hpp>

namespace lua
{
	namespace
	{
		utils::hook::detour lua_init_hook;
		utils::hook::detour lua_new_state_hook;

		bool loading_custom_script = false;

		vars::var_ptr var_lua_logging;
		vars::var_ptr var_lua_dump;

		void load_script(const std::string& name, const std::string& data)
		{
			loading_custom_script = true;
			const auto _0 = gsl::finally([&]
			{
				loading_custom_script = false;
			});

			const auto container = *game::lua::state_container;
			game::lua::luaL_loadbuffer(container->state, data.data(), data.size(), name.data());
		}

		void print_loading_script(const std::string& name)
		{
			console::info("Loading custom lua script '%s'\n", name.data());
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
				if (std::filesystem::is_directory(script) && utils::io::read_file(script + "/__init__.lua", &data))
				{
					print_loading_script(script);
					load_script(script + "/__init__.lua", data);
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
		}
	};
}

REGISTER_COMPONENT(lua::component)
