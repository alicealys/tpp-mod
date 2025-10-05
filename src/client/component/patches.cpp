#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "vars.hpp"

#include <utils/hook.hpp>
#include <utils/flags.hpp>
#include <utils/string.hpp>

namespace patches
{
	namespace
	{
		vars::var_ptr var_worker_count;
		vars::var_ptr var_unlock_fps;
		vars::var_ptr var_skip_intro;
		vars::var_ptr var_player_ramble_speed_scale;
		vars::var_ptr var_player_ramble_speed_patch;

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

			return var_worker_count->latched.get_int();
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

		HANDLE create_mutex_stub(LPSECURITY_ATTRIBUTES attributes, BOOL owner, LPCWSTR name)
		{
			return CreateMutexW(attributes, owner, NULL);
		}

		float scale_ramble_speed(float speed)
		{
			if (var_unlock_fps->latched.enabled() && var_player_ramble_speed_patch->current.enabled())
			{
				constexpr auto frame_time_60fps = 1000.0 / (60.0 * 1000.0);
				const auto time_system = game::fox::GetTimeSystem();
				const auto frame_time_scale = static_cast<float>(time_system.frameTime / frame_time_60fps);
				return speed * frame_time_scale * var_player_ramble_speed_scale->current.get_float();
			}
			else
			{
				return speed;
			}
		}

		utils::hook::detour get_ramble_speed_hook;
		float get_ramble_speed_stub(void* a1)
		{
			const auto value = get_ramble_speed_hook.invoke<float>(a1);
			return scale_ramble_speed(value);
		}

		int strncpy_s_stub(char* dst, size_t size, char* src, size_t max_count)
		{
			return strncpy_s(dst, size, src, _TRUNCATE);
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			var_worker_count = vars::register_int("com_worker_count", 4, 0, std::numeric_limits<int>::max(), 
				vars::var_flag_saved | vars::var_flag_latched, "maxiumum number of job executor worker threads");

			var_unlock_fps = vars::register_bool("com_unlock_fps", false, vars::var_flag_saved | vars::var_flag_latched, "unlock fps");

			if (game::environment::is_tpp())
			{
				var_player_ramble_speed_scale = vars::register_float("player_ramble_speed_scale", 1.51f, 0.f, 5.f, vars::var_flag_saved, "player sleep wake up scale (while player is trying to wake up)");
				var_player_ramble_speed_patch = vars::register_bool("player_ramble_speed_patch", true, vars::var_flag_saved, "enable high fps player sleep wake up patch");

				var_skip_intro = vars::register_bool("ui_skip_intro", false, vars::var_flag_saved | vars::var_flag_latched, "skip intro splashscreens");
			}
		}

		void start() override
		{
			if (game::environment::is_mgo())
			{
				// /AppData 99c85cdbf2c837d50d37c82af2c837d5c12d5e80fbc837d5f2c837d5f2c837d5f2 (command line arg)
				utils::hook::jump(0x143AA8300, sub_143AA8300_stub);
			}
			else
			{
				if (var_skip_intro->current.enabled())
				{
					// disable intro splash screen
					utils::hook::jump(0x145E59910, 0x145E5991B);
				}

				get_ramble_speed_hook.create(0x1468DA3F0, get_ramble_speed_stub);

				utils::hook::nop(0x144D21F3E, 6);
				utils::hook::call(0x144D21F3E, strncpy_s_stub);
			}

			utils::hook::nop(SELECT_VALUE(0x142E4ED98, 0x1422339D8), 6);
			utils::hook::call(SELECT_VALUE(0x142E4ED98, 0x1422339D8), create_mutex_stub);
			
			// disable _purecall error
			utils::hook::set<std::uint8_t>(SELECT_VALUE(0x141A05B96, 0x141461F6A), 0xC3);

			if (!game::environment::is_dedi() && var_unlock_fps->latched.enabled())
			{
				unlock_fps();
			}
		}
	};
}

REGISTER_COMPONENT(patches::component)
