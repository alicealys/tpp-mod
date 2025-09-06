#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "vars.hpp"

#include <utils/hook.hpp>
#include <utils/flags.hpp>

namespace patches
{
	namespace
	{
		vars::var_ptr var_worker_count;
		vars::var_ptr var_unlock_fps;
		vars::var_ptr var_skip_intro;

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

		unsigned int get_processor_count()
		{
			const auto worker_count_opt = utils::flags::get_flag("worker-count");
			if (worker_count_opt.has_value())
			{
				const auto& worker_count = worker_count_opt.value();
				return std::atoi(worker_count.data());
			}

			return var_worker_count->latched.get<bool>();
		}

		unsigned int get_processor_count_stub()
		{
			static const auto count = get_processor_count();
			return count;
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
			
			if (game::environment::is_mgo())
			{
				utils::hook::call(0x14002AFAB, get_processor_count_stub);
			}
			else
			{
				utils::hook::jump(0x142F308E0, get_processor_count_stub);
			}

			utils::hook::call(SELECT_VALUE(0x142F2AD7F, 0x14241449F), thread_sleep);
			utils::hook::jump(SELECT_VALUE(0x14008130A, 0x14008195A), SELECT_VALUE(0x1400814D8, 0x140081B28)); // unlock framerate always
		}
	}

	class component final : public component_interface
	{
	public:
		void post_start() override
		{
			var_worker_count = vars::register_int("com_worker_count", 4, 0, std::numeric_limits<int>::max(), 
				vars::var_flag_saved | vars::var_flag_latched, "maxiumum number of job executor worker threads");

			var_unlock_fps = vars::register_bool("com_unlock_fps", false, vars::var_flag_saved | vars::var_flag_latched, "unlock fps");

			var_skip_intro = vars::register_bool("ui_skip_intro", false, vars::var_flag_saved | vars::var_flag_latched, "skip intro splashscreens");
		}

		void post_unpack() override
		{
			if (game::environment::is_mgo())
			{
				// /AppData 99c85cdbf2c837d50d37c82af2c837d5c12d5e80fbc837d5f2c837d5f2c837d5f2 (command line arg)
				utils::hook::jump(0x143AA8300, sub_143AA8300_stub);
			}
			else
			{
				if (var_skip_intro->current.get<bool>())
				{
					// disable intro splash screen
					utils::hook::jump(0x145E59910, 0x145E5991B);
				}
			}	

			if (utils::flags::has_flag("unlock-fps") || var_unlock_fps->latched.get<bool>())
			{
				unlock_fps();
			}
		}
	};
}

REGISTER_COMPONENT(patches::component)
