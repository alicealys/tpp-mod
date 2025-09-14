#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "../console.hpp"
#include "../filesystem.hpp"

#include <utils/io.hpp>
#include <utils/hook.hpp>
#include <utils/flags.hpp>

namespace lua::script_loading
{
	namespace
	{
		utils::hook::detour lua_init_hook;
		utils::hook::detour lua_new_state_hook;

		bool loading_custom_script = false;

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
			printf("[lua]: initializing\n");
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

			utils::io::write_file("tpp-mod/dump/lua/"s + name_, std::string(buffer, size));

			std::string data;
			if (filesystem::read_file(name_, &data))
			{
				printf("Overriding lua script \"%s\"", name);
				return lual_load_buffer_hook.invoke<int>(state, data.data(), data.size(), name);
			}
			else
			{
				if (utils::flags::has_flag("lua-debug"))
				{
					printf("Loading lua script \"%s\"", name);
				}

				return lual_load_buffer_hook.invoke<int>(state, buffer, size, name);
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void start() override
		{
			if (!game::environment::is_mgsv())
			{
				return;
			}

			lua_init_hook.create(0x14319C450, lua_init_stub);
			lual_load_buffer_hook.create(game::lua::luaL_loadbuffer, lual_load_buffer_stub);
		}
	};
}

REGISTER_COMPONENT(lua::script_loading::component)
