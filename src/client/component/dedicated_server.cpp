#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "scheduler.hpp"
#include "console.hpp"
#include "vars.hpp"
#include "session.hpp"
#include "filesystem.hpp"

#include "directx/directx.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace dedicated_server
{
	namespace
	{
		utils::hook::detour translate_messages_hook;

		__int64 gn_execute_stub()
		{
			return -1;
		}

		void translate_messages_stub(void* a1)
		{
			utils::hook::invoke<void>(0x1400710C0, a1);

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

		void update_console_title()
		{
			std::string title = "MGO Dedicated Server";

			auto player_count = -1;
			auto player_limit = -1;

			const auto get_player_count = [&]
			{
				const auto session = *game::s_pSession;
				if (session == nullptr)
				{
					return;
				}

				player_count = 0;
				player_limit = session->allMembers.size;

				for (auto i = 0; i < player_limit; i++)
				{
					const auto member = session->allMembers.members[i];
					if (member != nullptr && member->flags != 0)
					{
						++player_count;
					}
				}
			};

			get_player_count();
			if (player_count != -1)
			{
				title = utils::string::va("%s (%i / %i)", title.data(), player_count, player_limit);
			}

			SetConsoleTitle(title.data());
		}

		utils::hook::detour on_player_connect_hook;
		__int64 on_player_connect_stub(void* a1, unsigned char index)
		{
			if (index == 0)
			{
				return 0;
			}

			return on_player_connect_hook.invoke<__int64>(a1, index);
		}

		utils::hook::detour on_init_frame_hook;
		void* on_init_frame_stub(void* a1, void* a2)
		{
			const auto weather = *game::tpp::sys::WeatherManager_::m_instance;
			if (weather != nullptr && weather->clock != nullptr)
			{
				weather->clock->pause = true;
			}
			return on_init_frame_hook.invoke<void*>(a1, a2);
		}

		void run_frame()
		{
			static const char* ruleset_names[] =
			{
				"RULESET_STATE_INACTIVE",
				"RULESET_STATE_GAME_START",
				"RULESET_STATE_GET_MISSION_INFO",
				"RULESET_STATE_BRIEFING",
				"RULESET_STATE_ROUND_COUNTDOWN",
				"RULESET_STATE_ROUND_REGULAR_PLAY",
				"RULESET_STATE_ROUND_OVERTIME",
				"RULESET_STATE_ROUND_SUDDEN_DEATH",
				"RULESET_STATE_ROUND_END",
				"RULESET_STATE_ROUND_RESULTS",
				"RULESET_STATE_DISTRIBUTE_RESULTS",
				"RULESET_STATE_FINAL_RESULTS",
				"RULESET_STATE_MISSION_RESULTS",
				"RULESET_STATE_END_OF_MATCH_FLOW",
				"RULESET_STATE_GAME_END",
			};

			static auto prev_state = 0;
			static auto did_rotate = false;

			const auto ruleset = session::get_active_ruleset();
			if (ruleset == nullptr)
			{
				prev_state = 0;
				return;
			}

			if (prev_state != ruleset->state)
			{
				did_rotate = false;
				console::info("[RulesetManager] state changed: %s (%i)", ruleset_names[ruleset->state], ruleset->state);
			}

			prev_state = ruleset->state;

			if (ruleset->state >= 13 && ruleset->currentRound == 2 && 
				ruleset->unk1.__vftable->GetTimeSpentInCurrentState(&ruleset->unk1) > 40.f && !did_rotate)
			{
				did_rotate = true;
				game::s_mgoMatchMakingManager->state = 21;
			}
		}

		int ret_n1()
		{
			return -1;
		}

		int ret_1()
		{
			return 1;
		}

		int ret_0()
		{
			return 0;
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			filesystem::register_resource_file("config\\server.cfg", RESOURCE_SERVER_CFG);

			if (!game::environment::is_mgo() || !game::environment::is_dedi())
			{
				return;
			}

			directx::disable_d3d11();

			SetConsoleTitle("MGO Dedicated Server");
			scheduler::loop(update_console_title, scheduler::session, 1s);

			utils::hook::set<std::uint8_t>(0x140A9EAE0, 0xC3); // dont build scene
			utils::hook::set<std::uint8_t>(0x140A9D01C, 0xEB); // ^
			utils::hook::set<std::uint8_t>(0x140A9D082, 0xEB); // ^

			utils::hook::set<std::uint8_t>(0x140BDA030, 0xC3); // dont execute draw 2d

			utils::hook::set<std::uint8_t>(0x140A9CAA0, 0xC3); // dont add render plugins
			utils::hook::set<std::uint8_t>(0x140C20240, 0xC3); // ^
			utils::hook::jump(0x145CDDF30, ret_0); // ^

			utils::hook::jump(0x140B23AE0, ret_n1);  // textures
			utils::hook::jump(0x140B62FC0, ret_n1);  // ^
			utils::hook::set(0x140AA8A20, 0xC301B0); // ^
			utils::hook::set(0x140B237F0, 0xC301B0); // ^
			utils::hook::set(0x140B23810, 0xC300B0); // ^

			utils::hook::set<std::uint8_t>(0x140D07550, 0xC3); // sound
			utils::hook::set<std::uint8_t>(0x140CF0F60, 0xC3); // ^
			utils::hook::set<std::uint8_t>(0x140CE4AD0, 0xC3); // ^
			utils::hook::set<std::uint8_t>(0x1406DF410, 0xC3); // ^
			utils::hook::set<std::uint8_t>(0x140CF73F0, 0xC3); // ^

			// dont create window
			utils::hook::nop(0x14007122D, 5);
			utils::hook::set<std::uint8_t>(0x140070E00, 0xC3);
			translate_messages_hook.create(0x140071900, translate_messages_stub);

			on_player_connect_hook.create(0x140829CB0, on_player_connect_stub); // dont spawn host
			on_init_frame_hook.create(0x1408271C0, on_init_frame_stub); // fix weather clock

			utils::hook::jump(0x14057F560, 0x14057F5D0); // always go to next match

			scheduler::loop(run_frame, scheduler::main);
		}
	};
}

REGISTER_COMPONENT(dedicated_server::component)
