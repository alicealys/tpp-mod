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
		game::match_settings_t match_settings{};

		struct match_field_t
		{
			int size;
			int offset;
		};

#define DEFINE_MATCH_FIELD(__struct__, __name__) \
		{#__name__, match_field_t(sizeof(__struct__::__name__), offsetof(__struct__, __name__))}

		std::unordered_map<std::string, match_field_t> match_settings_fields =
		{
			DEFINE_MATCH_FIELD(game::match_settings_t, match_type),
			DEFINE_MATCH_FIELD(game::match_settings_t, match_rule),
			DEFINE_MATCH_FIELD(game::match_settings_t, match_variant),
			DEFINE_MATCH_FIELD(game::match_settings_t, map_id),
			DEFINE_MATCH_FIELD(game::match_settings_t, day_night),
			DEFINE_MATCH_FIELD(game::match_settings_t, skill_level),
			DEFINE_MATCH_FIELD(game::match_settings_t, cheat_rate),
			DEFINE_MATCH_FIELD(game::match_settings_t, member_max),
			DEFINE_MATCH_FIELD(game::match_settings_t, unique_char),
			DEFINE_MATCH_FIELD(game::match_settings_t, walker_gear),
			DEFINE_MATCH_FIELD(game::match_settings_t, rank),
			DEFINE_MATCH_FIELD(game::match_settings_t, has_password),
		};

		std::unordered_map<std::string, match_field_t> match_rules_fields =
		{
			DEFINE_MATCH_FIELD(game::match_rules_t, pl_match_num),
			DEFINE_MATCH_FIELD(game::match_rules_t, pl_current_match),
			DEFINE_MATCH_FIELD(game::match_rules_t, pl_match_type),
			DEFINE_MATCH_FIELD(game::match_rules_t, pl_member_min),
			DEFINE_MATCH_FIELD(game::match_rules_t, pl_is_dedicated_host),
			DEFINE_MATCH_FIELD(game::match_rules_t, pl_auto_leave),
			DEFINE_MATCH_FIELD(game::match_rules_t, pl_dlc),
			DEFINE_MATCH_FIELD(game::match_rules_t, pl_briefing_time),
			DEFINE_MATCH_FIELD(game::match_rules_t, pl_total_match),
		};

		std::unordered_map<std::string, match_field_t> match_slot_fields =
		{
			DEFINE_MATCH_FIELD(game::match_slot_t, m_match_rule),
			DEFINE_MATCH_FIELD(game::match_slot_t, m_map_id),
			DEFINE_MATCH_FIELD(game::match_slot_t, m_variant),
			DEFINE_MATCH_FIELD(game::match_slot_t, m_dn),
			DEFINE_MATCH_FIELD(game::match_slot_t, m_time_limit),
			DEFINE_MATCH_FIELD(game::match_slot_t, m_tickets),
			DEFINE_MATCH_FIELD(game::match_slot_t, m_unique_char),
			DEFINE_MATCH_FIELD(game::match_slot_t, m_unique_char_solid),
			DEFINE_MATCH_FIELD(game::match_slot_t, m_unique_char_liquid),
			DEFINE_MATCH_FIELD(game::match_slot_t, m_walker_gear),
			DEFINE_MATCH_FIELD(game::match_slot_t, m_weather_change),
		};

		void create_lobby(game::mgo_match_t* match, game::match_settings_t* settings)
		{
			std::memcpy(&match->match_settings, settings, sizeof(game::match_settings_t));
			utils::hook::invoke<void>(0x1405A1970, match, &match->match_settings);
		}

		void set_field(const void* struct_, const match_field_t field, const int value)
		{
			const auto ptr = reinterpret_cast<size_t>(struct_) + field.offset;
			switch (field.size)
			{
			case 1:
				*reinterpret_cast<char*>(ptr) = static_cast<char>(value);
				break;
			case 2:
				*reinterpret_cast<char*>(ptr) = static_cast<char>(value);
				break;
			case 4:
				*reinterpret_cast<char*>(ptr) = static_cast<char>(value);
				break;
			}
		}

		int read_field(const void* struct_, const match_field_t field)
		{
			const auto ptr = reinterpret_cast<size_t>(struct_) + field.offset;
			switch (field.size)
			{
			case 1:
				return static_cast<int>(*reinterpret_cast<char*>(ptr));
			case 2:
				return static_cast<int>(*reinterpret_cast<char*>(ptr));
			case 4:
				return static_cast<int>(*reinterpret_cast<char*>(ptr));
			}

			return 0;
		}

		void set_slot_field(int slot_number, const std::string& field, const int value)
		{
			const auto iter = match_slot_fields.find(field);
			if (slot_number >= 5 || iter == match_slot_fields.end())
			{
				return;
			}

			set_field(&match_settings.rules.slots[slot_number], iter->second, value);
		}

		void set_match_setting(const std::string& field, const int value)
		{
			const auto iter = match_settings_fields.find(field);
			if (iter == match_settings_fields.end())
			{
				return;
			}

			set_field(&match_settings, iter->second, value);
		}

		void set_match_rule(const std::string& field, const int value)
		{
			const auto iter = match_rules_fields.find(field);
			if (iter == match_rules_fields.end())
			{
				return;
			}

			set_field(&match_settings.rules, iter->second, value);
		}

		void update_match_settings()
		{
			if (game::s_MgoMatchmakingManager->match_container == nullptr)
			{
				return;
			}

			std::memcpy(&game::s_MgoMatchmakingManager->match_container->match->match_settings, &match_settings, sizeof(game::match_settings_t));
		}

		std::atomic_bool request_match_start = false;
		std::atomic_bool request_match_rotate = false;
		std::atomic_bool request_disconnect = false;

		void run_frame()
		{
			static auto prev_state = 0;

			if (game::s_MgoMatchmakingManager->match_container == nullptr || game::s_MgoMatchmakingManager->state == 0)
			{
				return;
			}

			if (prev_state != game::s_MgoMatchmakingManager->state)
			{
				console::info("[matchmaking] State updated: %i\n", game::s_MgoMatchmakingManager->state);
			}

			prev_state = game::s_MgoMatchmakingManager->state;

			if (request_match_start && game::s_MgoMatchmakingManager->state == 2)
			{
				console::info("[matchmaking] Starting match...\n");

				request_match_start = false;
				create_lobby(game::s_MgoMatchmakingManager->match_container->match, &match_settings);
				game::s_MgoMatchmakingManager->state = 11;
			}

			if (request_match_rotate && (game::s_MgoMatchmakingManager->state == 20 || game::s_MgoMatchmakingManager->state == 19))
			{
				console::info("[matchmaking] Rotating match...\n");

				utils::hook::invoke<void>(0x148D00CE0);
				game::s_MgoMatchmakingManager->state = 21;
			}

			if (request_disconnect)
			{
				utils::hook::invoke<void>(0x140891C80, game::s_MgoMatchmakingManager.get(), 1);
			}

			request_match_rotate = false;
			request_disconnect = false;
		}

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
			if (!game::environment::is_mgo())
			{
				return;
			}

			scheduler::loop(run_frame, scheduler::main);

			command::add("disconnect", [](const command::params& params)
			{
				request_disconnect = true;
			});

			command::add("matchstart", [](const command::params& params)
			{
				request_match_start = true;
			});

			command::add("matchrotate", [](const command::params& params)
			{
				request_match_rotate = true;
			});

			command::add("matchset", [](const command::params& params)
			{
				if (params.size() < 3)
				{
					printf("usage: matchset <name> <value>\n");
					return;
				}

				set_match_setting(params.get(1), params.get_int(2));
			});

			command::add("matchsetrule", [](const command::params& params)
			{
				if (params.size() < 3)
				{
					printf("usage: matchsetrule <name> <value>\n");
					return;
				}

				set_match_rule(params.get(1), params.get_int(2));
			});

			command::add("matchsetslot", [](const command::params& params)
			{
				if (params.size() < 3)
				{
					printf("usage: matchsetslot <index> <name> <value>\n");
					return;
				}

				set_slot_field(params.get_int(1), params.get(2), params.get_int(3));
			});

			command::add("matchprint", []()
			{
				if (game::s_MgoMatchmakingManager->match_container == nullptr)
				{
					return;
				}

				const auto match = game::s_MgoMatchmakingManager->match_container->match;

				for (const auto& entry : match_settings_fields)
				{
					console::info("matchset %s %i\n", entry.first.data(), read_field(&match->match_settings, entry.second));
				}

				console::info("\n");

				for (const auto& entry : match_rules_fields)
				{
					console::info("matchsetrule %s %i\n", entry.first.data(), read_field(&match->match_rules, entry.second));
				}

				console::info("\n");

				for (auto i = 0; i < 5; i++)
				{
					for (const auto& entry : match_slot_fields)
					{
						const auto slot = &game::s_MgoMatchmakingManager->match_container->match->match_rules.slots[i];
						console::info("matchsetslot %i %s %i\n", i, entry.first.data(), read_field(slot, entry.second));
					}

					console::info("\n");
				}
			});
		}

		void start() override
		{
			if (!game::environment::is_dedi())
			{
				return;
			}

			utils::hook::set<std::uint8_t>(0x140A9F880, 0xC3); // dont build scene
			utils::hook::set<std::uint8_t>(0x14930E490, 0xC3); // dont render scene

			utils::hook::set<std::uint8_t>(0x14A91F2C0, 0xC3); // kill sound
			utils::hook::set<std::uint8_t>(0x140CE5630, 0xC3);
			utils::hook::set<std::uint8_t>(0x146579190, 0xC3);
			utils::hook::set<std::uint8_t>(0x14A91F2C0, 0xC3);
			utils::hook::set<std::uint8_t>(0x140E1A4B0, 0xC3);

			utils::hook::jump(0x141451F40, gn_execute_stub);
			utils::hook::nop(0x14258DC10, 5);
			translate_messages_hook.create(0x142590640, translate_messages_stub);

		}
	};
}

REGISTER_COMPONENT(dedicated_server::component)
