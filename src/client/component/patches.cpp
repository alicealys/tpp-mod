#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/flags.hpp>
#include <utils/nt.hpp>

namespace patches
{
	namespace
	{
		void set_timer_resolution()
		{
			ULONG data{};
			const utils::nt::library ntdll("ntdll.dll");
			ntdll.invoke_pascal<void>("NtSetTimerResolution", 5000, TRUE, &data);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!utils::flags::has_flag("unlock-fps"))
			{
				return;
			}

			set_timer_resolution();

			utils::hook::call(SELECT_VALUE(0x142F2AD7F, 0x14241449F), std::this_thread::yield); // remove weird sleep
			utils::hook::jump(SELECT_VALUE(0x14008130A, 0x14008195A), SELECT_VALUE(0x1400814D8, 0x140081B28)); // unlock framerate always
		}
	};
}

REGISTER_COMPONENT(patches::component)
