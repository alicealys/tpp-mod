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
		vars::var_ptr var_sensitivity_patch;
		vars::var_ptr var_camera_fovscale;
		vars::var_ptr var_camera_fp_fovscale;
		vars::var_ptr var_camera_fp_preserve;

		void set_timer_resolution()
		{
			ULONG data{};
			const utils::nt::library ntdll("ntdll.dll");
			ntdll.invoke_pascal<void>("NtSetTimerResolution", 5000, TRUE, &data);
		}

		void thread_sleep()
		{
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
			utils::hook::invoke<unsigned int*>(SELECT_VALUE_LANG(0x1402DCF10, 0x0), &data, unk + 288, value, sizeof(value) - 1);
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

			while ((std::chrono::steady_clock::now() - scheduler::main_frame_begin) < target_frame_time)
			{	
				std::this_thread::sleep_for(1ms);
			}
		}

		void job_pool_configuration_stub(__int64 a1)
		{
			*reinterpret_cast<int*>(a1 + 8) = 1;
			*reinterpret_cast<unsigned __int64*>(a1) = 0x3F89A02900419A03ull;
			*reinterpret_cast<int*>(a1 + 12) = 49152;
			*reinterpret_cast<__int64*>(a1 + 24) = 0;
			*reinterpret_cast<__int64*>(a1 + 32) = 0;
			*reinterpret_cast<int*>(a1 + 16) = 1000;
			*reinterpret_cast<int*>(a1 + 8) = get_processor_count_stub();
		}

		void unlock_fps()
		{
			set_timer_resolution();
			
			if (game::environment::is_mgo())
			{
				utils::hook::call(SELECT_VALUE_LANG(0x14002AA2B, 0x0), get_processor_count_stub);
				utils::hook::jump(SELECT_VALUE_LANG(0x14002AE80, 0x0), job_pool_configuration_stub);
			}
			else
			{
				utils::hook::jump(SELECT_VALUE_LANG(0x140034260, 0x0), get_processor_count_stub);
			}

			utils::hook::call(SELECT_VALUE(0x140032C9C, 0x14003287C, 0x0, 0x0), thread_sleep);

			// unlock framerate always
			utils::hook::jump(SELECT_VALUE(0x1400810AA, 0x1400822CA, 0x0, 0x0), SELECT_VALUE(0x140081278, 0x140082498, 0x0, 0x0));

			leave_frame_hook.create(SELECT_VALUE(0x14007FDB0, 0x140081060, 0x0, 0x0), leave_frame_stub);
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

		utils::hook::detour get_persona_name_hook;
		const char* get_persona_name_stub(game::ISteamFriends* this_)
		{
			static char buffer[0x200]{};
			const auto& name = var_name->current.get_string();
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

		void player_mouse_event_update_stub(__int64 a1)
		{
			const auto time_system = game::fox::GetTimeSystem();
			const auto frame_time_scale = static_cast<float>(time_system.frameTime) * 100.f;

			const auto v_x = *reinterpret_cast<LONG*>(a1 + 40);
			const auto v_y = *reinterpret_cast<LONG*>(a1 + 44);

			InterlockedExchangeAdd(reinterpret_cast<LONG*>(a1 + 40), -v_x);
			InterlockedExchangeAdd(reinterpret_cast<LONG*>(a1 + 44), -v_y);

			*reinterpret_cast<void**>(a1 + 48) = *reinterpret_cast<void**>(a1 + 32);

			if (var_sensitivity_patch->current.enabled())
			{
				*reinterpret_cast<float*>(a1 + 56) = static_cast<float>(v_x) * 0.001f * frame_time_scale;
				*reinterpret_cast<float*>(a1 + 60) = static_cast<float>(v_y) * 0.001f * frame_time_scale;
			}
			else
			{
				*reinterpret_cast<float*>(a1 + 56) = static_cast<float>(v_x) * 0.001f;
				*reinterpret_cast<float*>(a1 + 60) = static_cast<float>(v_y) * 0.001f;
			}
		}

		void patch_sensitivity()
		{
			constexpr const auto base_value = 0.016683333f;
			static auto value = base_value;

			utils::hook::inject(SELECT_VALUE(0x141206ECC, 0x1411B7CAB, 0x0, 0x0) + 4, &value);

			var_sensitivity->set_callback = []()
			{
				value = base_value * var_sensitivity->current.get_float();
			};

			var_sensitivity->set_callback->operator()();

			utils::hook::jump(SELECT_VALUE(0x1409CCB60, 0x1407AFAC0, 0x0, 0x0), player_mouse_event_update_stub);
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
			scale_fov(*reinterpret_cast<game::tpp::gm::player::impl::PlayerCameraImpl**>(a4 + 8), var_camera_fp_fovscale);
		}

		utils::hook::detour player_camera_set_tps_params_hook;
		void player_camera_set_tps_params_stub(game::tpp::gm::player::impl::PlayerCameraImpl* camera, void* params)
		{
			player_camera_set_tps_params_hook.invoke<void>(camera, params);
			scale_fov(camera, var_camera_fovscale);
		}

		utils::hook::detour player_camera_set_around_params_hook;
		void player_camera_set_around_params_stub(game::tpp::gm::player::impl::PlayerCameraImpl* camera, void* params)
		{
			player_camera_set_around_params_hook.invoke<void>(camera, params);
			scale_fov(camera, var_camera_fovscale);
		}

		bool check_update_fov()
		{
			const auto changed = var_camera_fovscale->changed;
			var_camera_fovscale->changed = false;
			return changed;
		}

		bool check_update_fov_fps()
		{
			const auto changed = var_camera_fp_fovscale->changed;
			var_camera_fp_fovscale->changed = false;
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
			a.jmp(SELECT_VALUE(0x14101DDA5, 0x1410159B3, 0x0, 0x0));

			a.bind(no_update);
			a.jmp(SELECT_VALUE(0x14101DDCC, 0x1410159DA, 0x0, 0x0));
		}

		void tps_camera_update_parameter_stub(utils::hook::assembler& a)
		{
			const auto no_update = a.new_label();

			a.sub(rsp, 0x28);
			a.mov(rax, qword_ptr(rcx, 8));
			a.mov(edx, qword_ptr(rax, SELECT_VALUE_NOLANG(0x204, 0x1F4)));
			a.shr(edx, 2);

			a.push(rax);
			a.pushad64();
			a.call_aligned(check_update_fov);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);

			a.and_(dl, 1);
			a.or_(dl, al);
			a.test(dl, dl);
			a.jz(no_update);
			a.jmp(SELECT_VALUE(0x1412BD4E6, 0x141222B86, 0x0, 0x0));

			a.bind(no_update);
			a.xor_(al, al);
			a.add(rsp, 0x28);
			a.ret();
		}

		void subjective_camera_update_parameter_stub(utils::hook::assembler& a)
		{
			const auto loop = a.new_label();
			const auto no_update = a.new_label();

			a.ror(rbx, 1);
			a.sub(rdi, 8);
			a.dec(esi);
			a.jns(loop);

			a.push(rax);
			a.pushad64();
			a.call_aligned(check_update_fov_fps);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);

			a.or_(bpl, al);
			a.test(bpl, bpl);
			a.jz(no_update);

			a.jmp(SELECT_VALUE(0x1412A06D3, 0x141214863, 0x0, 0x0));

			a.bind(loop);
			a.jmp(SELECT_VALUE(0x1412A0694, 0x141214824, 0x0, 0x0));

			a.bind(no_update);
			a.jmp(SELECT_VALUE(0x1412A06DB, 0x14121486B, 0x0, 0x0));
		}

		utils::hook::detour subjective_camera_set_default_hook;
		void subjective_camera_set_default_stub(void* a1, char a2)
		{
			if (var_camera_fp_preserve->current.enabled())
			{
				return;
			}

			subjective_camera_set_default_hook.invoke<void>(a1, a2);
		}

		void patch_fov()
		{
			subjective_camera_set_parameter_hook.create(SELECT_VALUE(0x14105AE20, 0x14104BBA0, 0x0, 0x0), subjective_camera_set_parameter_stub);
			player_camera_set_tps_params_hook.create(SELECT_VALUE(0x14111B5D0, 0x141103400, 0x0, 0x0), player_camera_set_tps_params_stub);
			player_camera_set_around_params_hook.create(SELECT_VALUE(0x14111AAA0, 0x1411028D0, 0x0, 0x0), player_camera_set_around_params_stub);
			subjective_camera_set_default_hook.create(SELECT_VALUE(0x14129F170, 0x141213490, 0x0, 0x0), subjective_camera_set_default_stub);

			utils::hook::jump(SELECT_VALUE(0x14101DD99, 0x1410159A5, 0x0, 0x0), utils::hook::assemble(around_camera_update_parameter_stub), true);
			utils::hook::jump(SELECT_VALUE(0x1412BD4D0, 0x141222B70, 0x0, 0x0), utils::hook::assemble(tps_camera_update_parameter_stub), true);
			utils::hook::jump(SELECT_VALUE(0x1412A06C4, 0x141214854, 0x0, 0x0), utils::hook::assemble(subjective_camera_update_parameter_stub), true);
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			var_worker_count = vars::register_int("com_worker_count", 4, 2, std::thread::hardware_concurrency(),
				vars::var_flag_saved | vars::var_flag_latched, "maxiumum number of job executor worker threads");

			var_unlock_fps = vars::register_bool("com_unlock_fps", false, 
				vars::var_flag_saved | vars::var_flag_latched, "unlock fps");

			if (game::environment::is_dedi())
			{
				var_max_fps = vars::register_int("com_max_fps", 100, 0, 1000, 0, "server max fps");
			}
			else
			{
				var_max_fps = vars::register_int("com_max_fps", 0, 0, 1000,
					vars::var_flag_saved, "max fps (only works when com_unlock_fps is enabled)");
			}

			var_sensitivity = vars::register_float("sensitivity", 1.f, 0.f, 10.f, 
				vars::var_flag_saved, "mouse sensitivity scale");

			var_sensitivity_patch = vars::register_bool("sensitivity_fps_patch", false, 
				vars::var_flag_saved, "enable sensitivity scaling patch");

			var_camera_fovscale = vars::register_float("camera_fovscale", 1.f, 0.1f, 5.f, 
				vars::var_flag_saved, "camera fov scale");

			var_camera_fp_fovscale = vars::register_float("camera_fp_fovscale", 1.f, 0.1f, 5.f, 
				vars::var_flag_saved, "first person camera fov scale");

			var_camera_fp_preserve = vars::register_bool("camera_fp_preserve", false, 
				vars::var_flag_saved, "preserve first person camera mode after leaving ADS");

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

					const auto& name = var_name->current.get_string();
					strncpy_s(inst->ptr1->ptr1->ptr2->name, sizeof(inst->ptr1->ptr1->ptr2->name), name.data(), name.size());
				};
			}
		}

		void start() override
		{
			if (game::environment::is_mgo())
			{
				// /AppData 99c85cdbf2c837d50d37c82af2c837d5c12d5e80fbc837d5f2c837d5f2c837d5f2 (command line arg)
				utils::hook::jump(SELECT_VALUE_LANG(0x1402DD2D0, 0x143A76D50), sub_143AA8300_stub);
			}
			else
			{
				if (var_skip_intro->current.enabled())
				{
					// disable intro splash screen
					utils::hook::jump(SELECT_VALUE_LANG(0x14090F210, 0x0), SELECT_VALUE_LANG(0x14090F21B, 0x0));
				}

				get_ramble_speed_hook.create(SELECT_VALUE_LANG(0x140AFD550, 0x0), get_ramble_speed_stub);
			}

			utils::hook::nop(SELECT_VALUE(0x1400013F9, 0x1400014E9, 0x0, 0x0), 6);
			utils::hook::call(SELECT_VALUE(0x1400013F9, 0x1400014E9, 0x0, 0x0), create_mutex_stub);
			
			// disable _purecall error
			utils::hook::set<std::uint8_t>(SELECT_VALUE(0x141A05976, 0x1414617EA, 0x0, 0x0), 0xC3);

			if (var_unlock_fps->latched.enabled() || game::environment::is_dedi())
			{
				unlock_fps();
			}

			patch_sensitivity();
			patch_fov();
		}
	};
}

REGISTER_COMPONENT(patches::component)
