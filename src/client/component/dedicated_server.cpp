#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "scheduler.hpp"
#include "console.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace dedicated_server
{
	namespace
	{
		void start_match(int location, int mode, int dn)
		{
			if (*reinterpret_cast<unsigned int*>(0x142055770) == 0)
			{
				*reinterpret_cast<unsigned int*>(0x142055770) |= 1u;
				utils::hook::invoke<void>(0x1408906F0, 0x142054190);
				utils::hook::invoke<void>(0x141462070, 0x1416E5B90);
			}

			utils::hook::invoke<void>(0x1408980D0, 0x142054190, 0, location, mode, dn);
		}

		void** thing()
		{
			static void* ptr = nullptr;
			return &ptr;
		}
	}

	class component final : public component_interface
	{
	public:
		void start() override
		{
			if (!game::environment::is_mgo())
			{
				return;
			}

			command::add("start_match", [](const command::params& params)
			{
				if (params.size() < 4)
				{
					printf("usage: start_match <location> <mode> <day_night>\n");
					return;
				}

				start_match(params.get_int(1), params.get_int(2), params.get_int(3));
			});

			if (!game::environment::is_dedi())
			{
				return;
			}

			utils::hook::set<std::uint8_t>(0x148CDB16F + 6, 0); // briefing time
			utils::hook::set<std::uint16_t>(0x148CDB114 + 7, 1); // dedicated host
			utils::hook::set<std::uint32_t>(0x148CDB127 + 1, 0); // min players
			utils::hook::set<std::uint32_t>(0x148CDB054 + 6, 2); // lobby type

			utils::hook::set<std::uint8_t>(0x140A9F880, 0xC3); // dont build scene
			utils::hook::set<std::uint8_t>(0x14930E490, 0xC3); // dont render scene

			utils::hook::set<std::uint8_t>(0x14A91F2C0, 0xC3); // kill sound
			utils::hook::set<std::uint8_t>(0x140CE5630, 0xC3);
			utils::hook::set<std::uint8_t>(0x146579190, 0xC3);
			utils::hook::set<std::uint8_t>(0x14A91F2C0, 0xC3);
			utils::hook::set<std::uint8_t>(0x140E1A4B0, 0xC3);
		}
	};
}

REGISTER_COMPONENT(dedicated_server::component)
