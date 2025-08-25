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

		void thread_sleep()
		{
			std::this_thread::yield();
		}

		unsigned int get_processor_count_stub()
		{
			return 4;
		}

		void sub_143AA8300_stub(__int64 unk, unsigned int* res, int arg_count, const char** args)
		{
			unsigned int data{};
			const char value[] = "99c85cdbf2c837d50d37c82af2c837d5c12d5e80fbc837d5f2c837d5f2c837d5f2";
			utils::hook::invoke<unsigned int*>(0x1402DCD30, &data, unk + 288, value, sizeof(value) - 1);
			*res = 0;
		}

		void unlock_fps()
		{
			set_timer_resolution();

			utils::hook::call(SELECT_VALUE(0x142F2AD7F, 0x14241449F), thread_sleep);
			utils::hook::jump(SELECT_VALUE(0x142F308E0, 0x14C6D6A20), get_processor_count_stub);

			utils::hook::jump(SELECT_VALUE(0x14008130A, 0x14008195A), SELECT_VALUE(0x1400814D8, 0x140081B28)); // unlock framerate always
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_mgo())
			{
				// /AppData 99c85cdbf2c837d50d37c82af2c837d5c12d5e80fbc837d5f2c837d5f2c837d5f2 (command line arg)
				utils::hook::jump(0x143AA8300, sub_143AA8300_stub);
			}

			if (utils::flags::has_flag("unlock-fps"))
			{
				unlock_fps();
			}
		}
	};
}

REGISTER_COMPONENT(patches::component)
