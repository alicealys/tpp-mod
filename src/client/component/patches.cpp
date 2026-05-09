#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "vars.hpp"
#include "scheduler.hpp"

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
		vars::var_ptr var_name;
		vars::var_ptr var_max_fps;
		vars::var_ptr var_sensitivity;
		vars::var_ptr var_camera_fov_scale;
		vars::var_ptr var_camera_fist_person_fov_scale;

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
			utils::hook::invoke<unsigned int*>(SELECT_VALUE_LANG(0x1402DCD30, 0x1402DCB70), &data, unk + 288, value, sizeof(value) - 1);
			*res = 0;
		}

		utils::hook::detour leave_frame_hook;

		void leave_frame_stub(void* a1)
		{
			leave_frame_hook.invoke<void>(a1);

			const auto max_fps = var_max_fps->current.get_int();
			if (max_fps == 0)
			{
				return;
			}

			constexpr auto nano_secs = std::chrono::duration_cast<std::chrono::nanoseconds>(1s);
			const auto target_frame_time = nano_secs / max_fps;
			const auto frame_time = std::chrono::steady_clock::now() - scheduler::main_frame_begin;
			const auto diff = target_frame_time - frame_time;

			if (diff > 0ms)
			{	
				std::this_thread::sleep_for(diff);
			}
		}

		void unlock_fps()
		{
			set_timer_resolution();
			
			if (game::environment::is_mgo())
			{
				utils::hook::call(SELECT_VALUE_LANG(0x14002AFAB, 0x14002AFCB), get_processor_count_stub);
				utils::hook::call(SELECT_VALUE_LANG(0x142361744, 0x1422E6BE4), get_processor_count_stub);
			}
			else
			{
				utils::hook::jump(SELECT_VALUE_LANG(0x142F308E0, 0x142F1E260), get_processor_count_stub);
			}

			utils::hook::call(SELECT_VALUE(0x142F2AD7F, 0x14241449F, 0x142F18E1F, 0x1423F700F), thread_sleep);

			// unlock framerate always
			utils::hook::jump(SELECT_VALUE(0x14008130A, 0x14008195A, 0x14008142A, 0x14008181A), 
				SELECT_VALUE(0x1400814D8, 0x140081B28, 0x1400815F8, 0x1400819E8));

			leave_frame_hook.create(SELECT_VALUE(0x1431882F0, 0x1425CA340, 0x14320E630, 0x1426DABC0), leave_frame_stub);
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

		utils::hook::detour get_persona_name_hook;
		const char* get_persona_name_stub(game::ISteamFriends* this_)
		{
			static char buffer[0x200]{};
			const auto name = var_name->current.get_string();
			strncpy_s(buffer, sizeof(buffer), name.data(), name.size());
			return buffer;
		}

		void set_player_name_once()
		{
			const auto steam_friends = (*game::SteamFriends)();
			const auto name = steam_friends->__vftable->GetPersonaName(steam_friends);

			var_name->reset = name;

			if (var_name->current.get_string().empty())
			{
				vars::set_var(var_name, name, vars::var_source_internal);
			}

			get_persona_name_hook.create(steam_friends->__vftable->GetPersonaName, get_persona_name_stub);
		}

		void patch_sensitivity()
		{
			constexpr const auto base_value = 0.016683333f;
			static auto value = base_value;

			utils::hook::inject(SELECT_VALUE(0x149A977B4, 0x1411B869B, 0x142ADAE18, 0x141EE0A14) + 4, &value);

			var_sensitivity->set_callback = []()
			{
				value = base_value * var_sensitivity->current.get_float();
			};

			var_sensitivity->set_callback->operator()();
		}

		void scale_fov(game::tpp::gm::player::impl::PlayerCameraImpl* camera, vars::var_ptr var)
		{
			const auto scale = (1.f / var->current.get_float());
			if (game::environment::is_tpp())
			{
				camera->tpp.fov *= scale;
			}
			else
			{
				camera->mgo.fov *= scale;
			}
		}

		utils::hook::detour subjective_camera_set_parameter_hook;
		void subjective_camera_set_parameter_stub(void* a1, void* a2, void* a3, __int64 a4)
		{
			subjective_camera_set_parameter_hook.invoke<void>(a1, a2, a3, a4);
			scale_fov(*reinterpret_cast<game::tpp::gm::player::impl::PlayerCameraImpl**>(a4 + 8), var_camera_fist_person_fov_scale);
		}

		utils::hook::detour player_camera_set_tps_params_hook;
		void player_camera_set_tps_params_stub(game::tpp::gm::player::impl::PlayerCameraImpl* camera, void* params)
		{
			player_camera_set_tps_params_hook.invoke<void>(camera, params);
			scale_fov(camera, var_camera_fov_scale);
		}

		utils::hook::detour player_camera_set_around_params_hook;
		void player_camera_set_around_params_stub(game::tpp::gm::player::impl::PlayerCameraImpl* camera, void* params)
		{
			player_camera_set_around_params_hook.invoke<void>(camera, params);
			scale_fov(camera, var_camera_fov_scale);
		}

		bool check_update_fov()
		{
			const auto changed = var_camera_fov_scale->changed;
			var_camera_fov_scale->changed = false;
			return changed;
		}

		void around_camera_update_parameter_stub(utils::hook::assembler& a)
		{
			const auto do_update = a.new_label();
			const auto no_update = a.new_label();

			if (game::environment::is_tpp())
			{
				a.and_(edi, 0xF0);
			}
			else
			{
				a.mov(r15, qword_ptr(rsp, 0xB8));
			}

			a.push(rax);
			a.pushad64();
			a.call_aligned(check_update_fov);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);

			a.test(al, al);
			a.jnz(do_update);

			a.cmp(rdi, qword_ptr(rbx, 0x18));
			a.jz(no_update);

			a.bind(do_update);
			a.jmp(SELECT_VALUE(0x14101E5A5, 0x141016463, 0x14101E5F5, 0x141015B43));

			a.bind(no_update);
			a.jmp(SELECT_VALUE(0x14101E5CC, 0x14101648A, 0x14101E61C, 0x141015B6A));
		}

		void patch_fov()
		{
			subjective_camera_set_parameter_hook.create(SELECT_VALUE(0x14105B660, 0x14104C650, 0x14105B6B0, 0x14104BD20), subjective_camera_set_parameter_stub);
			player_camera_set_tps_params_hook.create(SELECT_VALUE(0x1498447A0, 0x14BE550C0, 0x14A25F300, 0x14BD71D40), player_camera_set_tps_params_stub);
			player_camera_set_around_params_hook.create(SELECT_VALUE(0x14983F7D0, 0x14BE4EB00, 0x14A25BB10, 0x14BD6C270), player_camera_set_around_params_stub);

			utils::hook::jump(SELECT_VALUE(0x14101E599, 0x141016455, 0x14101E5E9, 0x141015B35), utils::hook::assemble(around_camera_update_parameter_stub), true);
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			var_worker_count = vars::register_int("com_worker_count", 4, 2, std::numeric_limits<int>::max(),
				vars::var_flag_saved | vars::var_flag_latched, "maxiumum number of job executor worker threads");

			var_unlock_fps = vars::register_bool("com_unlock_fps", false, 
				vars::var_flag_saved | vars::var_flag_latched, "unlock fps");

			var_max_fps = vars::register_int("com_max_fps", 0, 0, 1000, 
				vars::var_flag_saved, "max fps (only works when com_unlock_fps is enabled)");

			var_sensitivity = vars::register_float("sensitivity", 1.f, 0.f, 10.f, 
				vars::var_flag_saved, "mouse sensitivity scale");

			var_camera_fov_scale = vars::register_float("camera_fov_scale", 1.f, 0.1f, 5.f, 
				vars::var_flag_saved, "camera fov scale");

			var_camera_fist_person_fov_scale = vars::register_float("camera_first_person_fov_scale", 1.f, 0.1f, 5.f, 
				vars::var_flag_saved, "first person camera fov scale");

			if (game::environment::is_tpp())
			{
				var_player_ramble_speed_scale = vars::register_float("player_ramble_speed_scale", 1.51f, 0.f, 5.f, 
					vars::var_flag_saved, "player sleep wake up scale (while player is trying to wake up)");
				var_player_ramble_speed_patch = vars::register_bool("player_ramble_speed_patch", true, 
					vars::var_flag_saved, "enable high fps player sleep wake up patch");

				var_skip_intro = vars::register_bool("ui_skip_intro", false, 
					vars::var_flag_saved | vars::var_flag_latched, "skip intro splashscreens");
			}
			else
			{
				var_name = vars::register_string("name", "", vars::var_flag_saved, "Player name");
				scheduler::once(set_player_name_once, scheduler::main);
				var_name->set_callback = []()
				{
					const auto inst = game::tpp::net::Daemon_::GetInstance();
					if (inst == nullptr || inst->ptr1 == nullptr || inst->ptr1->ptr1 == nullptr ||
						inst->ptr1->ptr1->ptr2 == nullptr)
					{
						return;
					}

					const auto name = var_name->current.get_string();
					strncpy_s(inst->ptr1->ptr1->ptr2->name, sizeof(inst->ptr1->ptr1->ptr2->name), name.data(), name.size());
				};
			}
		}

		void start() override
		{
			if (game::environment::is_mgo())
			{
				// /AppData 99c85cdbf2c837d50d37c82af2c837d5c12d5e80fbc837d5f2c837d5f2c837d5f2 (command line arg)
				utils::hook::jump(SELECT_VALUE_LANG(0x143AA8300, 0x143A76D50), sub_143AA8300_stub);
			}
			else
			{
				if (var_skip_intro->current.enabled())
				{
					// disable intro splash screen
					utils::hook::jump(SELECT_VALUE_LANG(0x145E59910, 0x147A635B0), SELECT_VALUE_LANG(0x145E5991B, 0x147A635BB));
				}

				get_ramble_speed_hook.create(SELECT_VALUE_LANG(0x1468DA3F0, 0x1484C25F0), get_ramble_speed_stub);

				utils::hook::nop(SELECT_VALUE_LANG(0x144D21F3E, 0x144B8861D), 6);
				utils::hook::call(SELECT_VALUE_LANG(0x144D21F3E, 0x144B8861D), strncpy_s_stub);
			}

			utils::hook::nop(SELECT_VALUE(0x142E4ED98, 0x1422339D8, 0x142E4F8E8, 0x142232258), 6);
			utils::hook::call(SELECT_VALUE(0x142E4ED98, 0x1422339D8, 0x142E4F8E8, 0x142232258), create_mutex_stub);
			
			// disable _purecall error
			utils::hook::set<std::uint8_t>(SELECT_VALUE(0x141A05B96, 0x141461F6A, 0x141A05CB6, 0x141461E0A), 0xC3);

			if (!game::environment::is_dedi() && var_unlock_fps->latched.enabled())
			{
				unlock_fps();
			}

			patch_sensitivity();
			patch_fov();
		}
	};
}

REGISTER_COMPONENT(patches::component)
