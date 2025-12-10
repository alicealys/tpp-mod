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
		utils::hook::detour read_staff_data_hook;

		vars::var_ptr var_staff_cheat;

		void set_value(const size_t address, const int bit_offset, const int bits, const int value)
		{
			const auto byte_offset = bit_offset / 8;
			const auto mask = (1 << bits) - 1;
			const auto shifted_value = (value & mask) << (bit_offset % 8);

			auto original = *reinterpret_cast<int*>(address + byte_offset);
			original &= ~(mask << (bit_offset % 8));

			const auto new_value = original | shifted_value;
			*reinterpret_cast<int*>(address + byte_offset) = new_value;
		}

		void set_bit_field(const size_t address, int start_bit, int end_bit, int value)
		{
			set_value(address, start_bit, (end_bit - start_bit) + 1, value);
		}

		enum rank_t
		{
			rank_e = 0,
			rank_d = 1,
			rank_c = 2,
			rank_b = 3,
			rank_a = 4,
			rank_ap = 5,
			rank_app = 6,
			rank_s = 7,
			rank_sp = 8,
			rank_spp = 9,
		};

		enum designation_t
		{
			des_none = 0,
			des_units_start = 1,
			des_combat = 1,
			des_rnd = 2,
			des_base_dev = 3,
			des_support = 4,
			des_intel = 5,
			des_medical = 6,
			des_security = 7,
			des_sickbay = 8,
			des_units_end = 8,
			des_brig = 9,
			des_quarantine = 10,
			des_waiting_room_1 = 11,
			des_waiting_room_2 = 12,
			des_waiting_room_3 = 13,
			des_waiting_room_4 = 14,
			des_waiting_room_5 = 15,
			des_count,
		};

		enum skill_t
		{
			skill_gunman = 1,
			skill_climber = 2,
			skill_athlete = 3,
			skill_rescuer = 5,
			skill_quick_reload = 6,
			skill_tough_guy = 7,
			skill_fortunate = 8,
			skill_savage = 9,
			skill_boaster = 10,
			skill_botanist = 11,
			skill_quick_draw = 12,
			skill_vanguard_sharpshooter = 13,
			skill_prosthetic_mobility_1 = 14,
			skill_prosthetic_mobility_2 = 15,
			skill_prosthetic_mobility_3 = 16,
			skill_prosthetic_precision_movement_1 = 17,
			skill_prosthetic_precision_movement_2 = 18,
			skill_prosthetic_precision_movement_3 = 19,
			skill_prosthetic_first_aid_function_1 = 20,
			skill_prosthetic_first_aid_function_2 = 21,
			skill_prosthetic_first_aid_function_3 = 22,
			skill_gunsmith_handguns = 23,
			skill_gunsmith_submachine_guns = 24,
			skill_gunsmith_assault_rifles = 25,
			skill_gunsmith_shotguns = 26,
			skill_gunsmith_grenade_launchers = 27,
			skill_gunsmith_sniper_rifles = 28,
			skill_gunsmith_machine_guns = 29,
			skill_gunsmith_missile_launchers = 30,
			skill_master_gunsmith = 31,
			skill_anesthesia_specialist = 32,
			skill_noise_suppression_specialist = 33,
			skill_missile_guidance_specialist = 34,
			skill_zoologist = 35,
			skill_sleeping_gas_specialist = 36,
			skill_trap_specialist = 37,
			skill_electromagnetc_net_specialist = 38,
			skill_radar_specialist = 40,
			skill_metamaterials_specialist = 41,
			skill_drug_developer = 42,
			skill_bionics_specialist = 43,
			skill_mechatronics_specialist = 44,
			skill_cybernetics_specialist = 45,
			skill_rocket_control_specialist = 46,
			skill_electrospinning_specialist = 47,
			skill_materials_engineer = 48,
			skill_transportation_specialist = 49,
			skill_video_surveillance_specialist = 51,
			skill_bipedal_weapons_developer = 54,
			skill_interpreter_russian = 55,
			skill_interpreter_afrikaans = 56,
			skill_interpreter_kikongo = 57,
			skill_interpreter_pashto = 58,
			skill_surgeon = 59,
			skill_physician = 60,
			skill_counselor = 61,
			skill_parasitologist = 62,
			skill_troublemaker_violence = 63,
			skill_troublemaker_unsanitary = 64,
			skill_troublemaker_harassment = 65,
			skill_diplomat = 66,
			skill_tactical_instructor = 67,
			skill_mother_base_xo = 68,
			skill_required_end = 69,
			skill_defender_1 = 69,
			skill_defender_2 = 70,
			skill_defender_3 = 71,
			skill_sentry_1 = 72,
			skill_sentry_2 = 73,
			skill_sentry_3 = 74,
			skill_ranger_1 = 75,
			skill_ranger_2 = 76,
			skill_ranger_3 = 77,
			skill_medic_1 = 78,
			skill_medic_2 = 79,
			skill_medic_3 = 80,
			skill_liquid_carbon_missile_engineer_1 = 81,
			skill_liquid_carbon_missile_engineer_2 = 82,
			skill_liquid_carbon_missile_engineer_3 = 83,
			skill_anti_ballistic_missile_engineer_1 = 84,
			skill_anti_ballistic_missile_engineer_2 = 85,
			skill_anti_ballistic_missile_engineer_3 = 86,
		};

		enum stat_distribution_t
		{
			stat_dist_none_1 = 0b11010,
			stat_dist_none_2 = 0b11011,
			stat_dist_none_3 = 0b11100,
			stat_dist_none_4 = 0b11101,
			stat_dist_none_5 = 0b100100,
			stat_dist_none_6 = 0b100101,
			stat_dist_none_7 = 0b100110,
			stat_dist_none_8 = 0b100111,
			stat_dist_none_9 = 0b111011,
			stat_dist_security = 0b1,
			stat_dist_base_dev_focus = 0b100,
			stat_dist_base_dev_and_combat = 0b110011,
			stat_dist_base_dev_and_intel = 0b10010,
			stat_dist_base_dev_plus_and_intel_plus = 0b110001,
			stat_dist_base_dev_and_medical = 0b10011,
			stat_dist_base_dev_plus_and_medical_plus = 0b110010,
			stat_dist_base_dev_and_rnd = 0b101110,
			stat_dist_base_dev_and_support_and_intel_and_medical = 0b100000,
			stat_dist_base_dev_and_support = 0b10001,
			stat_dist_base_dev_plus_and_support_plus = 0b110000,
			stat_dist_combat_focus = 0b10,
			stat_dist_combat_and_base_dev = 0b101111,
			stat_dist_combat_and_intel = 0b1001,
			stat_dist_combat_plus_and_intel_plus = 0b101001,
			stat_dist_combat_and_medical = 0b1010,
			stat_dist_combat_and_rnd = 0b1000,
			stat_dist_combat_and_support_and_intel_and_medical = 0b11111,
			stat_dist_combat_and_support = 0b101000,
			stat_dist_intel_focus = 0b110,
			stat_dist_intel_and_base_dev = 0b10000,
			stat_dist_intel_plus_and_base_dev_plus = 0b110110,
			stat_dist_intel_and_combat_and_support_and_medical = 0b100010,
			stat_dist_intel_and_combat = 0b1110,
			stat_dist_intel_plus_and_combat_plus = 0b111001,
			stat_dist_intel_and_medical = 0b111000,
			stat_dist_intel_and_rnd = 0b1111,
			stat_dist_intel_and_support = 0b111010,
			stat_dist_medical_focus = 0b111,
			stat_dist_medical_and_base_dev_and_support_and_intel = 0b100011,
			stat_dist_medical_and_base_dev = 0b11000,
			stat_dist_medical_plus_and_base_dev_plus = 0b111110,
			stat_dist_medical_and_combat = 0b10111,
			stat_dist_medical_plus_and_combat_plus = 0b111100,
			stat_dist_medical_and_intel = 0b110111,
			stat_dist_medical_and_rnd = 0b111101,
			stat_dist_medical_and_support = 0b11001,
			stat_dist_rnd_focus = 0b11,
			stat_dist_rnd_and_base_dev = 0b1011,
			stat_dist_rnd_plus_and_base_dev_plus = 0b101011,
			stat_dist_rnd_and_combat = 0b101010,
			stat_dist_rnd_and_medical = 0b1101,
			stat_dist_rnd_plus_and_medical_plus = 0b101101,
			stat_dist_rnd_and_support_and_intel_and_medical = 0b11110,
			stat_dist_rnd_and_support = 0b1100,
			stat_dist_rnd_plus_and_support_plus = 0b101100,
			stat_dist_support_focus = 0b101,
			stat_dist_support_and_combat = 0b10100,
			stat_dist_support_plus_and_combat_plus = 0b110100,
			stat_dist_support_and_intel = 0b10110,
			stat_dist_support_plus_and_intel_plus = 0b110101,
			stat_dist_support_and_rnd_and_intel_and_medical = 0b100001,
			stat_dist_special_character = 0b111111,
		};

		struct header_t
		{
			unsigned int suppress_stats : 1;
			unsigned int stat_bonus : 2;
			unsigned int peak_rank : 4;
			unsigned int stat_distribution : 6;
			unsigned int skill : 7;
			unsigned int face_gender : 10;
		};

		struct status_t
		{
			unsigned int combat_deployment_team : 4;
			unsigned int player_selected : 3;
			unsigned int direct_contract : 1;
			unsigned int proficiency : 4;
			unsigned int ds_medal : 1;
			unsigned int ds_cross : 1;
			unsigned int honor_medal : 1;
			unsigned int unk : 1;
			unsigned int symptomatic : 1;
			unsigned int health_level : 3;
			unsigned int health_state : 2;
			unsigned int morale : 4;
			unsigned int enemy : 1;
			unsigned int designation : 4;
			unsigned int unselectable : 1;
		};

		void modify_stats_internal(const size_t ptr, const size_t index)
		{
			const auto header_ptr = *reinterpret_cast<size_t*>(ptr + 0x9C78) + index * 4;
			const auto status_ptr = *reinterpret_cast<size_t*>(ptr + 0x9C88) + index * 4;

			const auto header = reinterpret_cast<header_t*>(header_ptr);
			const auto status = reinterpret_cast<status_t*>(status_ptr);

			header->peak_rank = rank_t::rank_spp;
			header->stat_bonus = 3;
			header->suppress_stats = 0;

			const auto type = (index % 3);
			switch (type)
			{
			case 0:
				header->face_gender = 350 + (index % 50);
				break;
			case 1:
				header->face_gender = 450 + (index % 30);
				break;
			case 2:
				header->face_gender = 500 + (index % 20);
				break;
			}

			status->proficiency = 15;
			status->ds_medal = 1;
			status->ds_cross = 1;
			status->honor_medal = 1;
			status->morale = 15;
			status->health_state = 0;
			status->health_level = 7;

			status->unselectable = 0;
			status->direct_contract = 0;
			status->enemy = 0;

			switch (status->designation)
			{
			case des_combat:
			case des_security:
				header->stat_distribution = stat_dist_combat_plus_and_intel_plus;
				break;
			case des_rnd:
				header->stat_distribution = stat_dist_rnd_plus_and_base_dev_plus;
				break;
			case des_base_dev:
				header->stat_distribution = stat_dist_base_dev_plus_and_intel_plus;
				break;
			case des_support:
				header->stat_distribution = stat_dist_support_plus_and_combat_plus;
				break;
			case des_intel:
				header->stat_distribution = stat_dist_intel_plus_and_combat_plus;
				break;
			case des_medical:
				header->stat_distribution = stat_dist_medical_plus_and_combat_plus;
				break;
			case des_brig:
				status->designation = des_waiting_room_1;
				break;
			}
		}

		void modify_stats_stub(const size_t ptr, const size_t index)
		{
			if (!var_staff_cheat->current.enabled())
			{
				return;
			}

			auto total_staff = 0;
			for (auto i = 0; i < 3500; i++)
			{
				const auto status_ptr = *reinterpret_cast<size_t*>(ptr + 0x9C88) + i * 4;
				const auto status = reinterpret_cast<status_t*>(status_ptr);

				if (status->designation >= des_units_start && status->designation < des_count)
				{
					++total_staff;
					modify_stats_internal(ptr, i);
				}
			}

			static std::vector<skill_t> required_skills =
			{
				{skill_ranger_3},
				{skill_sentry_3},
				{skill_defender_3},
				{skill_medic_3},
			};

			const auto staff_per_skill = total_staff / required_skills.size();
			auto assigned_skills = 0;
			auto current_skill = 0;
			auto staff_assigned = 0;

			for (auto i = 0; i < 3500; i++)
			{
				const auto status_ptr = *reinterpret_cast<size_t*>(ptr + 0x9C88) + i * 4;
				const auto status = reinterpret_cast<status_t*>(status_ptr);

				if (status->designation >= des_units_start && status->designation <= des_units_end)
				{
					const auto header_ptr = *reinterpret_cast<size_t*>(ptr + 0x9C78) + i * 4;
					const auto header = reinterpret_cast<header_t*>(header_ptr);

					header->skill = required_skills[current_skill];

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

		void something_stub(utils::hook::assembler& a)
		{
			a.pushad64();
			a.mov(rcx, rcx);
			a.mov(rdx, rdx);
			a.call_aligned(modify_stats_stub);
			a.popad64();

			a.mov(rax, qword_ptr(rcx, 0x9C88));
			a.mov(r15d, dword_ptr(rax, rdx, 2));
			a.mov(rax, qword_ptr(rcx, 0x9C90));
			a.jmp(SELECT_VALUE_LANG(0x14A5BD3D7, 0x14B05B797));
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			var_staff_cheat = vars::register_bool("staff_cheat", false, vars::var_flag_cheat, "max staff ranks and skills");
		}

		void start() override
		{
			if (!game::environment::is_tpp())
			{
				return;
			}
			
			utils::hook::jump(SELECT_VALUE_LANG(0x14A5BD39D, 0x14B05B75D), utils::hook::assemble(something_stub), true);
		}
	};
}

REGISTER_COMPONENT(staff::component)
