#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "console.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace security
{
	namespace
	{
		void shell_impl_active_shell_at_empty_work_stub(utils::hook::assembler& a)
		{
			const auto is_nullptr = a.new_label();
			const auto continue_ = a.new_label();

			a.mov(eax, 0xFE00);
			a.test(word_ptr(r13, 0x32), ax);
			a.mov(rax, r11);
			a.setnz(al);

			a.test(r14, r14);
			a.jz(is_nullptr);

			a.movzx(eax, word_ptr(r14, rax, 1));
			a.jmp(continue_);

			a.bind(is_nullptr);
			a.xor_(rax, rax);

			a.bind(continue_);
			a.movd(xmm13, eax);
			a.lea(eax, qword_ptr(rdi, -0x146));
			a.jmp(SELECT_VALUE_LANG(0x14125EC51, 0x0));
		}

		void sub_1407A7F70_stub(utils::hook::assembler& a)
		{
			const auto is_nullptr = a.new_label();

			a.mov(rcx, qword_ptr(rcx, 0x3D0));
			a.mov(rax, qword_ptr(rcx));
			a.call(qword_ptr(rax, 0x20));

			a.test(rax, rax);
			a.jz(is_nullptr);
			a.jmp(SELECT_VALUE_LANG(0x1407A7C3D, 0x0));

			a.bind(is_nullptr);
			a.mov(al, 1);
			a.jmp(SELECT_VALUE_LANG(0x1407A7CAA, 0x0));
		}

		utils::hook::detour fv2_resource_manager_get_model_hook;
		void* fv2_resource_manager_get_model_stub(void* a1, void* a2)
		{
			if (a1 == nullptr)
			{
				return nullptr;
			}

			return fv2_resource_manager_get_model_hook.invoke<void*>(a1, a2);
		}

		void sub_1411126C0_stub(utils::hook::assembler& a)
		{
			const auto l1 = a.new_label();
			static char buf[0x1000]{};

			a.mov(rax, qword_ptr(rcx));
			a.call(qword_ptr(rax, 0x158));
			a.movzx(r8d, word_ptr(rbp, -0x38));
			a.mov(r15d, 0x7FF);

			a.test(rax, rax);
			a.jnz(l1);
			a.mov(rax, &buf);

			a.bind(l1);
			a.jmp(SELECT_VALUE_LANG(0x141112E8D, 0x0));
		}

		void sub_14082CA70_stub(utils::hook::assembler& a)
		{
			const auto null = a.new_label();
			a.test(rdx, rdx);
			a.jz(null);

			a.mov(qword_ptr(rbp, -0x48), rdx);
			a.mov(rax, qword_ptr(rdx, 0x70));
			a.movzx(eax, word_ptr(rax, rbx, 1));
			a.jmp(SELECT_VALUE_LANG(0x14082D50B, 0x0));

			a.bind(null);
			a.jmp(SELECT_VALUE_LANG(0x14082D149, 0x0));
		}

		void patch_mgo_crashes()
		{
			utils::hook::jump(SELECT_VALUE_LANG(0x14125EC31, 0x0), utils::hook::assemble(shell_impl_active_shell_at_empty_work_stub), true);
			utils::hook::jump(SELECT_VALUE_LANG(0x1407A7C30, 0x0), utils::hook::assemble(sub_1407A7F70_stub), true);
			utils::hook::jump(SELECT_VALUE_LANG(0x141112E7C, 0x0), utils::hook::assemble(sub_1411126C0_stub), true);
			utils::hook::jump(SELECT_VALUE_LANG(0x14082D4FF, 0x0), utils::hook::assemble(sub_14082CA70_stub), true);
			fv2_resource_manager_get_model_hook.create(SELECT_VALUE_LANG(0x14029FE60, 0x0), fv2_resource_manager_get_model_stub);
		}

		utils::hook::detour json_get_hook;

		union fob_construct_param_t
		{
			struct
			{
				std::uint32_t unk1 : 1;
				std::uint32_t area : 7;
				std::uint32_t color_bits1 : 4;
				std::uint32_t layout : 10;
				std::uint32_t lat_long : 6;
				std::uint32_t color_bits2 : 1;
				std::uint32_t pad : 3;
			} fields;
			std::uint32_t packed;
		};

		void validate_construct_param(fob_construct_param_t& param)
		{
			param.fields.unk1 = 1;
			param.fields.pad = 0;

#define SNAP_VALUE(value, min, max, ceil) \
			if (value >= min && param.fields.layout <= ceil) \
			{ \
				value = std::clamp(param.fields.layout, min, max); \
			} \

			if (param.fields.layout >= 10 && param.fields.layout <= 110)
			{
				SNAP_VALUE(param.fields.layout, 10u, 13u, 19u);
				SNAP_VALUE(param.fields.layout, 20u, 23u, 29u);
				SNAP_VALUE(param.fields.layout, 30u, 33u, 39u);
				SNAP_VALUE(param.fields.layout, 40u, 43u, 49u);
				SNAP_VALUE(param.fields.layout, 50u, 53u, 59u);
				SNAP_VALUE(param.fields.layout, 60u, 63u, 69u);
				SNAP_VALUE(param.fields.layout, 70u, 73u, 79u);
				SNAP_VALUE(param.fields.layout, 80u, 83u, 89u);
				SNAP_VALUE(param.fields.layout, 90u, 93u, 99u);
				SNAP_VALUE(param.fields.layout, 100u, 103u, 110u);
			}
			else
			{
				param.fields.layout = 10;
			}

			if (param.fields.area >= 10 && param.fields.area < 80)
			{
				SNAP_VALUE(param.fields.layout, 10u, 11u, 19u);
				SNAP_VALUE(param.fields.layout, 20u, 21u, 29u);
				SNAP_VALUE(param.fields.layout, 30u, 32u, 39u);
				SNAP_VALUE(param.fields.layout, 40u, 42u, 49u);
				SNAP_VALUE(param.fields.layout, 50u, 53u, 59u);
				SNAP_VALUE(param.fields.layout, 60u, 63u, 69u);
				SNAP_VALUE(param.fields.layout, 70u, 71u, 79u);
			}
			else
			{
				param.fields.area = 10;
			}
		}

		void validate_common_security(game::Json::Value* j)
		{
			const auto do_value = [&](const char* key, int max)
			{
				const auto value = json_get_hook.invoke<game::Json::Value*>(j, key);
				if (value != nullptr && (value->type == 1 || value->type == 2))
				{
					value->u.integer = std::clamp(value->u.integer, 0, max);
				}
			};

			do_value("uav", 2);
			do_value("mine", 9);
			do_value("decoy", 9);
			do_value("camera", 7);
			do_value("soldier", 8);
			do_value("antitheft", 10);
			do_value("ir_sensor", 5);
		}

		void validate_unique_security(game::Json::Value* j)
		{
			const auto do_value = [&](const char* key, int max)
			{
				const auto value = json_get_hook.invoke<game::Json::Value*>(j, key);
				if (value != nullptr && (value->type == 1 || value->type == 2))
				{
					value->u.integer = std::clamp(value->u.integer, 0, max);
				}
			};

			do_value("uav", 4);
			do_value("mine", 12);
			do_value("decoy", 12);
			do_value("camera", 8);
			do_value("soldier", 12);
			do_value("antitheft", 22);
			do_value("ir_sensor", 5);
		}

		game::Json::Value* json_get_stub(game::Json::Value* j, const char* key)
		{
			const auto value = json_get_hook.invoke<game::Json::Value*>(j, key);
			if (value == nullptr)
			{
				return value;
			}

			if (value->type == 1 || value->type == 2)
			{
				if (key == "voluntary_coord_camera_count"s)
				{
					value->u.integer = std::min(1, value->u.integer);
				}

				if (key == "voluntary_coord_mine_count"s)
				{
					value->u.integer = std::min(4, value->u.integer);
				}

				if (key == "construct_param"s)
				{
					fob_construct_param_t param{};
					param.packed = value->u.integer;
					validate_construct_param(param);
					value->u.integer = param.packed;
				}
			}
			else if (value->type == 0 || value->type == 7)
			{
				if (key == "common1_security"s ||
					key == "common2_security"s ||
					key == "common3_security"s)
				{
					validate_common_security(value);
				}

				if (key == "unique_security"s)
				{
					validate_unique_security(value);
				}
			}

			return value;
		}

		void patch_fob_security()
		{
			json_get_hook.create(SELECT_VALUE_LANG(0x141A0B7D0, 0x0), json_get_stub);
		}

		int strncpy_s_stub(char* dst, size_t size, char* src, size_t max_count)
		{
			return strncpy_s(dst, size, src, _TRUNCATE);
		}

		int atoi_stub(const char* str)
		{
			const auto count = std::atoi(str);
			return std::clamp(count, 0, 16);
		}

		int lua_nullsub(void* state)
		{
			console::error("[Security] a script tried to call an unsafe lua function\n");
			return 0;
		}

		int lua_nullsub_module(void* state)
		{
			return 0;
		}

		int system_stub(const char* arg)
		{
			console::error("[Security] a script tried to call 'system' (%s)\n", arg);
			return 0;
		}

		int get_num_lobby_members_stub(game::ISteamMatchmaking* steam_matchmaking, game::steam_id lobby_id)
		{
			const auto num = steam_matchmaking->__vftable->GetNumLobbyMembers(steam_matchmaking, lobby_id);
			return std::clamp(num, 0, 16);
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			if (game::environment::is_mgo())
			{
				patch_mgo_crashes();

				// cap kick_num to 16
				utils::hook::nop(SELECT_VALUE_LANG(0x1405A29CE, 0x0), 6);
				utils::hook::call(SELECT_VALUE_LANG(0x1405A29CE, 0x0), atoi_stub);
				utils::hook::nop(SELECT_VALUE_LANG(0x1405D4AEE, 0x0), 6);
				utils::hook::call(SELECT_VALUE_LANG(0x1405D4AEE, 0x0), atoi_stub);

				// cap lobby member num to 16
				utils::hook::nop(SELECT_VALUE_LANG(0x1405D6BCE, 0x0), 7);
				utils::hook::call(SELECT_VALUE_LANG(0x1405D6BCE, 0x0), get_num_lobby_members_stub);
				utils::hook::nop(SELECT_VALUE_LANG(0x1405A633B, 0x0), 7);
				utils::hook::call(SELECT_VALUE_LANG(0x1405A633B, 0x0), get_num_lobby_members_stub);
			}
			else
			{
				patch_fob_security();

				// patch emblem crash exploit
				utils::hook::nop(SELECT_VALUE_LANG(0x1405597A1, 0x144B8861D), 6);
				utils::hook::call(SELECT_VALUE_LANG(0x1405597A1, 0x144B8861D), strncpy_s_stub);
			}

			utils::hook::jump(SELECT_VALUE(0x141A31640, 0x14148C9D0, 0x0, 0x0), lua_nullsub); // disable lua os func
			utils::hook::jump(SELECT_VALUE(0x141A31E90, 0x14148D220, 0x0, 0x0), lua_nullsub); // ^
			utils::hook::jump(SELECT_VALUE(0x141A317A0, 0x14148CB30, 0x0, 0x0), lua_nullsub); // ^
			utils::hook::jump(SELECT_VALUE(0x141A31680, 0x14148CA10, 0x0, 0x0), lua_nullsub); // ^
			utils::hook::jump(SELECT_VALUE(0x141A316D0, 0x14148CA60, 0x0, 0x0), lua_nullsub); // ^
			utils::hook::jump(SELECT_VALUE(0x141A31E20, 0x14148D1B0, 0x0, 0x0), lua_nullsub); // ^
			utils::hook::jump(SELECT_VALUE(0x141A31730, 0x14148CAC0, 0x0, 0x0), lua_nullsub); // ^
			utils::hook::jump(SELECT_VALUE(0x141A35A40, 0x141490DD0, 0x0, 0x0), lua_nullsub); // disable lua loadlib

			utils::hook::jump(SELECT_VALUE(0x141A310D0, 0x14148C460, 0x0, 0x0), lua_nullsub_module); // disable lua io module
			utils::hook::jump(SELECT_VALUE(0x141A35A10, 0x141490DA0, 0x0, 0x0), lua_nullsub_module); // disable lua debug module

			utils::hook::set(SELECT_VALUE(0x14208DC60, 0x1416F0AA0, 0x0, 0x0), system_stub); // remove 'system' import
		}
	};
}

REGISTER_COMPONENT(security::component)
