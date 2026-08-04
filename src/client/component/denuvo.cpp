#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "vars.hpp"
#include "scheduler.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/io.hpp>

namespace denuvo
{
	namespace
	{
		void restore_funcs()
		{

		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			// denuvo obfuscates game functions randomly at compile time,
			// each binary has a different set of obfuscated functions that can cause lag
			restore_funcs();
		}
	};
}

REGISTER_COMPONENT(denuvo::component)
