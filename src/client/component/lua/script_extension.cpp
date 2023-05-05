#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "../console.hpp"
#include "../filesystem.hpp"

#include <utils/io.hpp>
#include <utils/hook.hpp>

namespace lua::script_extension
{
	namespace
	{
		template <console::console_type Type>
		void lua_print(game::lua::lua_State* s)
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
			if (!game::environment::is_mgsv())
			{
				return;
			}

			utils::hook::inject(0x1435B49B6 + 3, lua_print<console::con_type_info>);
		}
	};
}

REGISTER_COMPONENT(lua::script_extension::component)
