#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "../console.hpp"
#include "../filesystem.hpp"

#include <utils/io.hpp>
#include <utils/hook.hpp>
#include <utils/flags.hpp>

namespace lua::script_extension
{
	namespace
	{
		template <console::console_type Type>
		void lua_print(lua_State* s)
		{
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
		void post_unpack() override
		{
			if (!utils::flags::has_flag("lua-debug") || !game::environment::is_mgsv())
			{
				return;
			}

			utils::hook::inject(0x1435B49B6 + 3, lua_print<console::con_type_info>);
			utils::hook::inject(0x1435B49D9 + 3, lua_print<console::con_type_warning>);
			utils::hook::inject(0x1435B4A03 + 3, lua_print<console::con_type_warning>);
			utils::hook::inject(0x1435B4A1F + 3, lua_print<console::con_type_error>);
		}
	};
}

REGISTER_COMPONENT(lua::script_extension::component)
