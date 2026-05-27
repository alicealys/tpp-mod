#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "scheduler.hpp"
#include "console.hpp"
#include "vars.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>
#include <utils/cryptography.hpp>

namespace staff
{
	namespace
	{
		vars::var_ptr var_cheat_unlockall_gear;
		vars::var_ptr var_cheat_unlockall_items;
		vars::var_ptr var_cheat_disable_reporting;
		vars::var_ptr var_cheat_develop_limit;

		const char* resource_names[] =
		{
			"FUEL_RESOURCE",
			"BIOTIC_RESOURCE",
			"COMMON_METAL",
			"MINOR_METAL",
			"PRECIOUS_METAL",
			"WORM_WOOD",
			"BLACK_CARROT",
			"GOLDEN_CRESCENT",
			"TARRAGON",
			"AFRICAN_PEACH",
			"DIGITALIS_P",
			"DIGITALIS_L",
			"HAOMA",
			"4WD_EAST",
			"4WD_WEST",
			"TRUCK_EAST",
			"TRUCK_WEST",
			"ARMORED_VEHICLE_EAST",
			"ARMORED_VEHICLE_WEST",
			"ARMORED_VEHICLE_EAST_ROCKET",
			"WHEELED_ARMORED_VEHICLE_WEST",
			"TANK_EAST",
			"TANK_WEST",
			"WALKER_GEAR_PROTO_HEUY",
			"WALKER_GEAR_SOVIET_BATTLE",
			"WALKER_GEAR_SOVIET_SUPPORT",
			"WALKER_GEAR_CFA_BATTLE",
			"WALKER_GEAR_CFA_SUPPORT",
			"NUCLEAR_WEAPON",
			"NUCLEAR_WASTE",
			"PARASITE_FOG",
			"PARASITE_CAMOFLA",
			"PARASITE_CURING",
			"PARASITE_RESERVE",
			"EMPLACEMENT_GUN_EAST",
			"EMPLACEMENT_GUN_WEST",
			"MORTAR_NORMAL",
			"ANTI_AIR_GATLING_GUN_EAST",
			"ANTI_AIR_GATLING_GUN_WEST",
			"CBOX_POSTER_1000",
			"CBOX_POSTER_1001",
			"CBOX_POSTER_1002",
			"CBOX_POSTER_1003",
			"CBOX_POSTER_1004",
			"CBOX_POSTER_1005",
			"CBOX_POSTER_1006",
			"RESERVE_A",
			"RESERVE_B",
			"RESERVE_C",
			"RESERVE_D",
			"RESERVE_E",
			"RESERVE_F",
			"RESERVE_G",
			"RESERVE_H",
			"RESERVE_I",
			"RESERVE_J",
			"RESERVE_K",
			"RESERVE_L",
			"RESERVE_M",
		};

		void modify_stats_internal(game::tpp::mbm::impl::StaffControllerImpl::StaffHeader* header,
			game::tpp::mbm::impl::StaffControllerImpl::StaffStatusSync* status, const std::uint32_t index)
		{
			header->fields.peak_rank = game::tpp::mbm::impl::StaffControllerImpl::RANK_SPP;
			header->fields.stat_bonus = 3;
			header->fields.suppress_stats = 0;

			const auto type = (index % 3);
			switch (type)
			{
			case 0:
				header->fields.face_gender = 350 + (index % 50);
				break;
			case 1:
				header->fields.face_gender = 450 + (index % 30);
				break;
			case 2:
				header->fields.face_gender = 500 + (index % 20);
				break;
			}

			status->fields.proficiency = 15;
			status->fields.ds_medal = 1;
			status->fields.ds_cross = 1;
			status->fields.honor_medal = 1;
			status->fields.morale = 15;
			status->fields.health_state = 0;
			status->fields.health_level = 7;

			status->fields.unselectable = 0;
			status->fields.direct_contract = 0;
			status->fields.enemy = 0;

			switch (status->fields.designation)
			{
			case game::tpp::mbm::impl::StaffControllerImpl::DESIGNATION_COMBAT:
			case game::tpp::mbm::impl::StaffControllerImpl::DESIGNATION_SECURITY:
				header->fields.stat_distribution = game::tpp::mbm::impl::StaffControllerImpl::STAT_DIST_COMBAT_PLUS_AND_INTEL_PLUS;
				break;
			case game::tpp::mbm::impl::StaffControllerImpl::DESIGNATION_RND:
				header->fields.stat_distribution = game::tpp::mbm::impl::StaffControllerImpl::STAT_DIST_RND_PLUS_AND_BASE_DEV_PLUS;
				break;
			case game::tpp::mbm::impl::StaffControllerImpl::DESIGNATION_BASE_DEV:
				header->fields.stat_distribution = game::tpp::mbm::impl::StaffControllerImpl::STAT_DIST_BASE_DEV_PLUS_AND_INTEL_PLUS;
				break;
			case game::tpp::mbm::impl::StaffControllerImpl::DESIGNATION_SUPPORT:
				header->fields.stat_distribution = game::tpp::mbm::impl::StaffControllerImpl::STAT_DIST_SUPPORT_PLUS_AND_COMBAT_PLUS;
				break;
			case game::tpp::mbm::impl::StaffControllerImpl::DESIGNATION_INTEL:
				header->fields.stat_distribution = game::tpp::mbm::impl::StaffControllerImpl::STAT_DIST_INTEL_PLUS_AND_COMBAT_PLUS;
				break;
			case game::tpp::mbm::impl::StaffControllerImpl::DESIGNATION_MEDICAL:
				header->fields.stat_distribution = game::tpp::mbm::impl::StaffControllerImpl::STAT_DIST_MEDICAL_PLUS_AND_COMBAT_PLUS;
				break;
			default:
			case game::tpp::mbm::impl::StaffControllerImpl::DESIGNATION_BRIG:
				status->fields.designation = game::tpp::mbm::impl::StaffControllerImpl::DESIGNATION_WAITING_ROOM_1;
				break;
			}
		}

		void modify_stats(game::tpp::mbm::impl::StaffControllerImpl::StaffHeader* header, game::tpp::mbm::impl::StaffControllerImpl::StaffStatusSync* status)
		{
			auto total_staff = 0;
			for (auto i = 0; i < 3500; i++)
			{
				if (status[i].fields.designation >= game::tpp::mbm::impl::StaffControllerImpl::DESIGNATION_UNITS_START &&
					status[i].fields.designation < game::tpp::mbm::impl::StaffControllerImpl::DESIGNATION_COUNT)
				{
					++total_staff;
					modify_stats_internal(&header[i], &status[i], i);
				}
			}

			static std::vector<std::uint32_t> required_skills =
			{
				{game::tpp::mbm::impl::StaffControllerImpl::SKILL_RANGER_3},
				{game::tpp::mbm::impl::StaffControllerImpl::SKILL_SENTRY_3},
				{game::tpp::mbm::impl::StaffControllerImpl::SKILL_DEFENDER_3},
				{game::tpp::mbm::impl::StaffControllerImpl::SKILL_MEDIC_3},
			};

			const auto staff_per_skill = total_staff / required_skills.size();
			auto assigned_skills = 0;
			auto current_skill = 0;
			auto staff_assigned = 0;

			for (auto i = 0; i < 3500; i++)
			{
				if (status[i].fields.designation >= game::tpp::mbm::impl::StaffControllerImpl::DESIGNATION_UNITS_START && 
					status[i].fields.designation <= game::tpp::mbm::impl::StaffControllerImpl::DESIGNATION_UNITS_END)
				{
					header[i].fields.skill = required_skills[current_skill];

					++assigned_skills;
					++staff_assigned;
					
					if (assigned_skills >= staff_per_skill && ((total_staff - staff_assigned) >= staff_per_skill))
					{
						assigned_skills = 0;
						++current_skill;
						if (current_skill >= required_skills.size())
						{
							break;
						}
					}
					else if (total_staff == staff_assigned)
					{
						break;
					}
				}
			}
		}

		game::tpp::mbm::impl::MotherBaseManagementSystemImpl* get_motherbase_sys()
		{
			const auto system_table = game::fox::GetQuarkSystemTable();
			if (system_table == nullptr ||
				system_table->applicationSystem == nullptr ||
				system_table->applicationSystem->motherBaseManagementSystem == nullptr)
			{
				return nullptr;
			}

			return system_table->applicationSystem->motherBaseManagementSystem;
		}

		void do_staff_cheat()
		{
			if (!vars::cheats_enabled())
			{
				console::error("cheats are not enabled\n");
				return;
			}

			const auto mb_sys = get_motherbase_sys();
			if (mb_sys == nullptr)
			{
				return;
			}

			modify_stats(
				mb_sys->staffController->mbmStaffSvarsHeaders,
				mb_sys->staffController->mbmStaffSvarsStatusesSync);
		}

		bool unlockall_items_enabled()
		{
			return var_cheat_unlockall_items->current.enabled();
		}

		bool unlockall_gear_enabled()
		{
			return var_cheat_unlockall_gear->current.enabled();
		}

		void get_purchasable_item_list_stub(utils::hook::assembler& a)
		{
			const auto continue_ = a.new_label();

			a.push(rax);
			a.pushad64();
			a.call_aligned(unlockall_gear_enabled);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);

			a.mov(r8d, dword_ptr(r8, 0x80));

			a.test(al, al);
			a.jz(continue_);
			a.mov(r8d, 1);

			a.bind(continue_);
			a.pop(rax);
			a.cmp(eax, 0x80);
			a.jmp(SELECT_VALUE_LANG(0x140598945, 0x140598325));
		}

		void cmd_get_server_item_list_result_unpack_stub(utils::hook::assembler& a)
		{
			const auto continue_ = a.new_label();
			const auto not_enabled = a.new_label();

			a.push(rax);
			a.pushad64();
			a.call_aligned(unlockall_items_enabled);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);

			a.mov(dword_ptr(rcx, rsi, 0, 0x5C), ebp);
			a.mov(dword_ptr(rcx, rsi, 0, 0x68), r12d);

			a.test(al, al);
			a.jz(not_enabled);

			a.mov(dword_ptr(rcx, rsi, 0, 0x60), 1); // open
			a.mov(dword_ptr(rcx, rsi, 0, 0x64), 2); // develop
			a.mov(dword_ptr(rcx, rsi, 0, 0x6C), 0); // left_second
			a.jmp(continue_);

			a.bind(not_enabled);
			a.mov(dword_ptr(rcx, rsi, 0, 0x60), r14d);
			a.mov(dword_ptr(rcx, rsi, 0, 0x64), r15d);
			a.mov(dword_ptr(rcx, rsi, 0, 0x6C), r13d);
			a.jmp(continue_);

			a.bind(continue_);
			a.jmp(SELECT_VALUE_LANG(0x1408335B4, 0x1408331E4));
		}

		int get_develop_limit()
		{
			return var_cheat_develop_limit->current.get_int();
		}

		void cmd_get_server_item_list_result_unpack_stub2(utils::hook::assembler& a)
		{
			a.call(SELECT_VALUE_LANG(0x141A0B8F0, 0x141A0BA10));
			a.mov(rcx, rax);
			a.call(SELECT_VALUE_LANG(0x141A0BEC0, 0x141A0BFE0));

			a.push(eax);
			a.push(rcx);
			a.pushad64();
			a.call_aligned(get_develop_limit);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rcx);
			a.pop(eax);

			a.test(ecx, ecx);
			a.cmovnz(eax, ecx);

			a.mov(dword_ptr(rsi, 0x2860), eax);
			a.jmp(SELECT_VALUE_LANG(0x14083362F, 0x14083325F));
		}

		utils::hook::detour cmd_check_server_item_correct_hook;
		char cmd_check_server_item_correct_stub(__int64 a1)
		{
			if (unlockall_items_enabled())
			{
				*reinterpret_cast<int*>(a1 + 80) = 0;
				*reinterpret_cast<int*>(a1 + 1364) = 0;
			}

			return cmd_check_server_item_correct_hook.invoke<char>(a1);
		}

		utils::hook::detour send_suspicion_play_data_hook;
		char send_suspicion_play_data_stub(void* a1, void* a2)
		{
			if (var_cheat_disable_reporting->current.enabled())
			{
				return 1;
			}

			return send_suspicion_play_data_hook.invoke<char>(a1, a2);
		}

		void cmd_add_resource(const command::params& params, bool processing)
		{
			if (!vars::cheats_enabled())
			{
				console::error("cheats are not enabled\n");
				return;
			}

			if (params.size() < 2)
			{
				const auto cmd = params.get(0);
				printf("usage: %s <resource index> <amount>\n", cmd.data());
				for (auto i = 0; i < 59; i++)
				{
					printf("%i: %s\n", i, resource_names[i]);
				}

				return;
			}

			const auto index = params.get_int(1);
			const auto value = params.get_int(2);
			if (index >= 59)
			{
				return;
			}

			const auto mb_sys = get_motherbase_sys();
			if (mb_sys == nullptr || mb_sys->resourceController == nullptr)
			{
				return;
			}

			const auto resource_controller = mb_sys->resourceController;
			if (processing)
			{
				resource_controller->processingResource[index].count += value;
			}
			else
			{
				auto resource = &resource_controller->usableResource[index];
				auto count = static_cast<int>(resource->count);
				if (resource->sign)
				{
					count *= -1;
				}

				count += value;
				resource->sign = count < 0;
				resource->count = std::abs(count);
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			if (game::environment::is_tpp())
			{
				command::add("cheat_spp_staff", do_staff_cheat);

				command::add("cheat_add_gmp", [](const command::params& params)
				{
					if (!vars::cheats_enabled())
					{
						console::error("cheats are not enabled\n");
						return;
					}

					if (params.size() < 2)
					{
						return;
					}

					const auto gmp = params.get_int(1);
					const auto mb_sys = get_motherbase_sys();
					if (mb_sys == nullptr)
					{
						return;
					}

					if (gmp < 0)
					{
						mb_sys->__vftable->SubTppGmp(mb_sys, gmp * -1);
					}
					else
					{
						mb_sys->__vftable->AddTppGmp(mb_sys, gmp);
					}
				});

				command::add("cheat_add_heroic_point", [](const command::params& params)
				{
					if (!vars::cheats_enabled())
					{
						console::error("cheats are not enabled\n");
						return;
					}

					if (params.size() < 2)
					{
						return;
					}

					const auto point = params.get_int(1);
					const auto mb_sys = get_motherbase_sys();
					if (mb_sys == nullptr)
					{
						return;
					}

					*mb_sys->heroicPoint = std::clamp(*mb_sys->heroicPoint + point, -99999999, 99999999);
				});

				command::add("cheat_set_ogre_point", [](const command::params& params)
				{
					if (!vars::cheats_enabled())
					{
						console::error("cheats are not enabled\n");
						return;
					}

					if (params.size() < 2)
					{
						return;
					}

					const auto point = params.get_int(1);
					const auto mb_sys = get_motherbase_sys();
					if (mb_sys == nullptr)
					{
						return;
					}

					*mb_sys->ogrePoint = std::clamp(point, -99999999, 99999999);
				});

				command::add("cheat_add_usable_resource", [](const command::params& params)
				{
					cmd_add_resource(params, false);
				});

				command::add("cheat_add_processing_resource", [](const command::params& params)
				{
					cmd_add_resource(params, true);
				});

				var_cheat_unlockall_items = vars::register_bool("cheat_unlockall_server_items", false,
					vars::var_flag_cheat | vars::var_flag_saved, "unlock all online items");

				var_cheat_disable_reporting = vars::register_bool("cheat_disable_reporting", false,
					vars::var_flag_cheat | vars::var_flag_saved, "disable reporting suspicious gameplay to konami");

				var_cheat_develop_limit = vars::register_int("cheat_develop_limit", 0, 0, vars::int_max,
					vars::var_flag_cheat | vars::var_flag_saved, "override online item develop limit (0 = disabled)");

				utils::hook::jump(SELECT_VALUE_LANG(0x14083359C, 0x1408331CC), utils::hook::assemble(cmd_get_server_item_list_result_unpack_stub), true);
				utils::hook::jump(SELECT_VALUE_LANG(0x14083361C, 0x14083324C), utils::hook::assemble(cmd_get_server_item_list_result_unpack_stub2), true);

				cmd_check_server_item_correct_hook.create(SELECT_VALUE_LANG(0x145B4DE40, 0x14752AD10), cmd_check_server_item_correct_stub);
			
				send_suspicion_play_data_hook.create(SELECT_VALUE_LANG(0x140809DD0, 0x140809A30), send_suspicion_play_data_stub);
			}
			else
			{
				var_cheat_unlockall_gear = vars::register_bool("cheat_unlockall_gear", false, 
					vars::var_flag_cheat | vars::var_flag_saved, "unlock all gear");

				utils::hook::jump(SELECT_VALUE_LANG(0x140598938, 0x140598318), utils::hook::assemble(get_purchasable_item_list_stub), true, true);
			}
		}

		void start() override
		{

		}
	};
}

REGISTER_COMPONENT(staff::component)
