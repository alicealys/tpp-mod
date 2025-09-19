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
		__int64 gn_execute_stub()
		{
			return -1;
		}

		utils::hook::detour translate_messages_hook;

		void translate_messages_stub(void* a1)
		{
			utils::hook::invoke<void>(0x140071410, a1);

			while (true)
			{
				const auto ptr = *reinterpret_cast<void**>(0x141F28320);
				if (ptr != nullptr)
				{
					utils::hook::invoke<void>(ptr, 0, 33281, 0, 0);
					utils::hook::invoke<void>(ptr, 0, 33281, 0, 0);
					break;
				}

				std::this_thread::sleep_for(1ms);
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{

		}

		void start() override
		{
			if (!game::environment::is_dedi())
			{
				return;
			}

			SetConsoleTitle("MGO Dedicated Server");

			utils::hook::set<std::uint8_t>(0x140A9F880, 0xC3); // dont build scene
			utils::hook::set<std::uint8_t>(0x149865F0B, 0xEB); // ^
			utils::hook::set<std::uint8_t>(0x14930E490, 0xC3); // dont render scene

			utils::hook::set<std::uint8_t>(0x14A91F2C0, 0xC3); // kill sound
			utils::hook::set<std::uint8_t>(0x140CE5630, 0xC3);
			utils::hook::set<std::uint8_t>(0x146579190, 0xC3);
			utils::hook::set<std::uint8_t>(0x14A91F2C0, 0xC3);
			utils::hook::set<std::uint8_t>(0x140E1A4B0, 0xC3);

			utils::hook::set<std::uint8_t>(0x14A1E39C0, 0xC3);

			utils::hook::jump(0x141451F40, gn_execute_stub);
			utils::hook::nop(0x14258DC10, 5);
			translate_messages_hook.create(0x142590640, translate_messages_stub);

		}
	};
}

REGISTER_COMPONENT(dedicated_server::component)
