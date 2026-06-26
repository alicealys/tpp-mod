#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "scheduler.hpp"
#include "console.hpp"
#include "matchmaking.hpp"

#include "text_chat/defs.hpp"
#include "text_chat/ui.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace matchmaking
{
	namespace
	{
		utils::hook::detour create_lobby_cb_hook;
		utils::hook::detour create_lobby_hook;

		game::match_settings_t match_settings{};

		vars::var_ptr var_match_enable_tweaks;
		vars::var_ptr var_match_min_players;
		vars::var_ptr var_match_max_players;
		vars::var_ptr var_match_briefing_time;
		vars::var_ptr var_match_password;

		struct match_field_t
		{
			int size;
			int offset;
		};

		struct kick_msg_t
		{
			std::uint32_t type;
			std::uint32_t unk;
			std::uint64_t steam_id;
		};

		std::unordered_set<std::uint64_t> kicked_steam_ids;

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
			DEFINE_MATCH_FIELD(game::match_settings_t, host_comment),
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
			create_lobby_hook.invoke<void>(match, &match->match_settings);
		}

		void update_match_settings()
		{
			const auto match_container = game::s_mgoMatchMakingManager->match_container;
			if (match_container == nullptr || match_container->match == nullptr)
			{
				return;
			}

			std::memcpy(&match_container->match->match_settings, &match_settings, sizeof(game::match_settings_t));
		}

		void create_lobby_stub(game::mgo_match_t* match, game::match_settings_t* settings)
		{
			if (var_match_enable_tweaks->current.enabled())
			{
				settings->rules.pl_member_min = static_cast<char>(var_match_min_players->current.get_int());
				settings->member_max = var_match_max_players->current.get_int();
				settings->rules.pl_briefing_time = static_cast<short>(var_match_briefing_time->current.get_int());
			}

			create_lobby_hook.invoke<void>(match, settings);
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
				*reinterpret_cast<short*>(ptr) = static_cast<short>(value);
				break;
			case 4:
				*reinterpret_cast<int*>(ptr) = value;
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
				return static_cast<int>(*reinterpret_cast<short*>(ptr));
			case 4:
				return static_cast<int>(*reinterpret_cast<int*>(ptr));
			}

			return 0;
		}

		void set_slot_field(int slot_number, const std::string& field, const int value)
		{
			const auto iter = match_slot_fields.find(field);
			if (slot_number >= 5 || iter == match_slot_fields.end())
			{
				console::warn("field \"%s\" does not exist\n", field.data());
				return;
			}

			set_field(&match_settings.rules.slots[slot_number], iter->second, value);
			update_match_settings();
		}

		void set_match_setting(const std::string& field, const int value)
		{
			const auto iter = match_settings_fields.find(field);
			if (iter == match_settings_fields.end())
			{
				console::warn("field \"%s\" does not exist\n", field.data());
				return;
			}

			set_field(&match_settings, iter->second, value);
			update_match_settings();
		}

		void set_match_rule(const std::string& field, const int value)
		{
			const auto iter = match_rules_fields.find(field);
			if (iter == match_rules_fields.end())
			{
				console::warn("field \"%s\" does not exist\n", field.data());
				return;
			}

			set_field(&match_settings.rules, iter->second, value);
			update_match_settings();
		}

		std::atomic_bool request_match_start = false;
		std::atomic_bool request_match_rotate = false;
		std::atomic_bool request_disconnect = false;

		void run_frame()
		{
			static auto prev_state = 0;

			if (game::s_mgoMatchMakingManager->match_container == nullptr || game::s_mgoMatchMakingManager->state == 0)
			{
				return;
			}

			if (prev_state != game::s_mgoMatchMakingManager->state)
			{
				console::debug("[MgoMatchmakingManager] State updated: %i\n", game::s_mgoMatchMakingManager->state);
			}

			if (prev_state > game::s_mgoMatchMakingManager->state)
			{
				text_chat::clear();
			}

			prev_state = game::s_mgoMatchMakingManager->state;

			if (request_match_start && game::s_mgoMatchMakingManager->state == 2)
			{
				console::info("[MgoMatchmakingManager] Starting match...\n");

				request_match_start = false;
				create_lobby(game::s_mgoMatchMakingManager->match_container->match, &match_settings);
				game::s_mgoMatchMakingManager->state = 11;
			}

			if (request_match_rotate && (game::s_mgoMatchMakingManager->state == 20 || game::s_mgoMatchMakingManager->state == 19))
			{
				console::info("[MgoMatchmakingManager] Rotating match...\n");

				game::s_mgoMatchMakingManager->__pad3[20] = 1;
				game::s_mgoMatchMakingManager->__pad4[4] = 1;
				game::s_mgoMatchMakingManager->__pad5[2] = 1;
				game::s_mgoMatchMakingManager->unk3 = 1;
			}

			if (request_disconnect)
			{
				utils::hook::invoke<void>(SELECT_VALUE_LANG(0x140892870, 0x140891740), game::s_mgoMatchMakingManager.get(), 1);
			}

			request_match_rotate = false;
			request_disconnect = false;
		}

		game::steam_id get_current_steam_id()
		{
			game::steam_id result{};
			const auto steam_user = (*game::SteamUser)();
			steam_user->__vftable->GetSteamID(steam_user, &result);
			return result;
		}

		void set_lobby_data(const std::string& key, const std::string& value)
		{
			static const auto current_steam_id = get_current_steam_id();
			const auto match_container = game::s_mgoMatchMakingManager->match_container;
			if (match_container == nullptr || 
				match_container->match->lobby_id.bits == 0 || 
				match_container->match->lobby_owner.bits != current_steam_id.bits)
			{
				return;
			}

			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			const auto res = steam_matchmaking->__vftable->SetLobbyData(steam_matchmaking, match_container->match->lobby_id, key.data(), value.data());
			console::debug("[SteamMatchmaking] SetLobbyData(%s, %s) = %i\n", key.data(), value.data(), res);
		}

		void set_lobby_data(const std::string& key, const std::uint64_t value)
		{
			set_lobby_data(key, utils::string::va("%llu", value));
		}

		const char* get_lobby_data(const std::string& key)
		{
			const auto match_container = game::s_mgoMatchMakingManager->match_container;
			if (match_container == nullptr)
			{
				return "";
			}

			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			return steam_matchmaking->__vftable->GetLobbyData(steam_matchmaking, match_container->match->lobby_id, key.data());
		}

		void update_kick_list()
		{
			const auto kick_num = std::min(16ull, kicked_steam_ids.size());
			set_lobby_data("kick_num", kicked_steam_ids.size());
			
			auto index = 0;
			for (const auto& id : kicked_steam_ids)
			{
				set_lobby_data(utils::string::va("kicked_id_%d", index++), id);
				if (index >= 16)
				{
					break;
				}
			}
		}

		void update_match_password()
		{
			const auto password = var_match_password->current.get_string();
			if (password.empty())
			{
				set_lobby_data("has_password", 0);
				set_lobby_data("password", "");
			}
			else
			{
				set_lobby_data("has_password", 1);
				set_lobby_data("password", password);
			}
		}

		void* create_lobby_cb_stub(void* a1, game::steam_id lobby_id)
		{
			printf("[SteamMatchmaking] Created lobby %llu\n", lobby_id.bits);
			const auto res = create_lobby_cb_hook.invoke<void*>(a1, lobby_id);
			update_kick_list();
			update_match_password();
			return res;
		}

		game::ISteamMatchmaking_vtbl steam_matchmaking_vtbl{};
		bool request_lobby_data_stub(game::ISteamMatchmaking* this_, game::steam_id lobby_id)
		{
			console::debug("[SteamMatchmaking] RequestLobbyData %lli\n", lobby_id.bits);
			return steam_matchmaking_vtbl.RequestLobbyData(this_, lobby_id);
		}

		unsigned __int64 join_lobby_stub(game::ISteamMatchmaking* this_, game::steam_id lobby_id)
		{
			console::info("[SteamMatchmaking] JoinLobby %lli", lobby_id.bits);
			text_chat::clear();
			return steam_matchmaking_vtbl.JoinLobby(this_, lobby_id);
		}

		void leave_lobby_stub(game::ISteamMatchmaking* this_, game::steam_id lobby_id)
		{
			console::info("[SteamMatchmaking] LeaveLobby %lli (%p)", lobby_id.bits, _ReturnAddress());
			text_chat::clear();
			return steam_matchmaking_vtbl.LeaveLobby(this_, lobby_id);
		}

		bool set_lobby_data_stub(game::ISteamMatchmaking* this_, game::steam_id lobby_id, const char* key, const char* value)
		{
			console::debug("[SteamMatchmaking] SetLobbyData %s %s\n", key, value);
			return steam_matchmaking_vtbl.SetLobbyData(this_, lobby_id, key, value);
		}

		const char* get_lobby_data_stub(game::ISteamMatchmaking* this_, game::steam_id lobby_id, const char* key)
		{
			console::debug("[SteamMatchmaking] GetLobbyData %s\n", key);
			return steam_matchmaking_vtbl.GetLobbyData(this_, lobby_id, key);
		}

		void add_request_lobby_list_numerical_filter(game::ISteamMatchmaking* this_, const char* key, int value, int compare)
		{
			console::debug("[SteamMatchmaking] AddRequestLobbyListNumericalFilter %s %i %i\n", key, value, compare);
			steam_matchmaking_vtbl.AddRequestLobbyListNumericalFilter(this_, key, value, compare);
		}

		void hook_steam_matchmaking()
		{
			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			std::memcpy(&steam_matchmaking_vtbl, steam_matchmaking->__vftable, sizeof(game::ISteamMatchmaking_vtbl));

			utils::hook::set(&steam_matchmaking->__vftable->JoinLobby, join_lobby_stub);
			utils::hook::set(&steam_matchmaking->__vftable->LeaveLobby, leave_lobby_stub);
			utils::hook::set(&steam_matchmaking->__vftable->RequestLobbyData, request_lobby_data_stub);
			utils::hook::set(&steam_matchmaking->__vftable->SetLobbyData, set_lobby_data_stub);
			//utils::hook::set(&steam_matchmaking->__vftable->GetLobbyData, get_lobby_data_stub);
			utils::hook::set(&steam_matchmaking->__vftable->AddRequestLobbyListNumericalFilter, add_request_lobby_list_numerical_filter);
		}

		int atoi_stub(const char* str)
		{
			auto count = std::atoi(str);
			if (count > 16)
			{
				count = 16;
			}
			return count;
		}
	}

	void ban_player_from_lobby(const std::uint64_t steam_id)
	{
		kicked_steam_ids.insert(steam_id);
		update_kick_list();
	}

	void unban_player_from_lobby(const std::uint64_t steam_id)
	{
		kicked_steam_ids.erase(steam_id);
		update_kick_list();
	}

	void kick_player_from_lobby(const std::uint64_t steam_id)
	{
		const auto match_container = game::s_mgoMatchMakingManager->match_container;
		if (match_container == nullptr)
		{
			return;
		}

		kick_msg_t kick_msg{};
		kick_msg.type = 1;
		kick_msg.unk = 0xFFFFFFFF;
		kick_msg.steam_id = steam_id;

		const auto steam_matchmaking = (*game::SteamMatchmaking)();
		steam_matchmaking->__vftable->SendLobbyChatMsg(steam_matchmaking, match_container->match->lobby_id, &kick_msg, sizeof(kick_msg));
	}

	void connect_to_lobby(game::steam_id lobby_id)
	{
		if (game::s_mgoMatchMakingManager->match_container == nullptr)
		{
			return;
		}

		const auto match = game::s_mgoMatchMakingManager->match_container->match;

		match->is_joining_invite = 1;
		match->invite_lobby_id = lobby_id;

		const auto steam_matchmaking = (*game::SteamMatchmaking)();
		steam_matchmaking->__vftable->RequestLobbyData(steam_matchmaking, lobby_id);
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

			var_match_enable_tweaks = vars::register_bool("match_enable_tweaks", false, vars::var_flag_saved, "enable match settings tweaks");
			var_match_min_players = vars::register_int("match_min_players", 2, 0, 16, vars::var_flag_saved, "match minimum players override");
			var_match_max_players = vars::register_int("match_max_players", 16, 0, 16, vars::var_flag_saved, "match maximum players override");
			var_match_briefing_time = vars::register_int("match_briefing_time", 60, 0, 600, vars::var_flag_saved, "match briefing time override (seconds)");
			var_match_password = vars::register_string("match_password", "", vars::var_flag_saved, "match password");

			var_match_password->set_callback = []()
			{
				scheduler::once(update_match_password, scheduler::session);
			};
		
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

			command::add("matchstart", [](const command::params& params)
			{
				request_match_start = true;
			});
		}

		void start() override
		{
			if (!game::environment::is_mgo())
			{
				return;
			}

			create_lobby_cb_hook.create(SELECT_VALUE_LANG(0x1405A18E0, 0x1466D0C80), create_lobby_cb_stub);
			create_lobby_hook.create(SELECT_VALUE_LANG(0x1405A1B60, 0x1405A1380), create_lobby_stub);

			// cap kick_num to 16
			utils::hook::nop(SELECT_VALUE_LANG(0x1405A29DE, 0x0), 6);
			utils::hook::call(SELECT_VALUE_LANG(0x1405A29DE, 0x0), atoi_stub); 
			utils::hook::nop(SELECT_VALUE_LANG(0x1405D4AEE, 0x0), 6);
			utils::hook::call(SELECT_VALUE_LANG(0x1405D4AEE, 0x0), atoi_stub);

			scheduler::once(hook_steam_matchmaking, scheduler::net);
			scheduler::loop(run_frame, scheduler::session);

			command::add("clearkicks", []()
			{
				scheduler::once([]
				{
					kicked_steam_ids.clear();
					set_lobby_data("kick_num", 0);
				}, scheduler::session);
			});

			command::add("connect_lobby", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					printf("usage: connect_lobby <lobby_id>\n");
					return;
				}

				game::steam_id lobby_id{};

				const auto lobby_id_s = params.get(1);
				lobby_id.bits = std::strtoull(lobby_id_s.data(), nullptr, 0);

				connect_to_lobby(lobby_id);
			});

			command::add("disconnect", [](const command::params& params)
			{
				request_disconnect = true;
			});

			command::add("reconnect", [](const command::params& params)
			{
				if (game::s_mgoMatchMakingManager->match_container == nullptr)
				{
					return;
				}

				const auto lobby_id = game::s_mgoMatchMakingManager->match_container->match->lobby_id;

				request_disconnect = true;
				scheduler::once([=]
				{
					connect_to_lobby(lobby_id);
				}, scheduler::main, 1s);
			});

			command::add("matchrotate", [](const command::params& params)
			{
				auto match = game::s_mgoMatchMakingManager->match_container;
				if (match == nullptr)
				{
					return;
				}

				const auto steam_matchmaking = (*game::SteamMatchmaking)();
				steam_matchmaking->__vftable->SetLobbyData(steam_matchmaking, 
					match->match->lobby_id2, "st_is_transition", "1");

				scheduler::once([&]
				{
					match = game::s_mgoMatchMakingManager->match_container;
					if (match == nullptr)
					{
						return;
					}

					request_match_rotate = true;
				}, scheduler::session, 500ms);
			});

			command::add("matchsetstate", [](const command::params& params)
			{
				const auto state = params.get_int(1);
				game::s_mgoMatchMakingManager->state = state;
			});

			command::add("matchprint", []()
			{
				if (game::s_mgoMatchMakingManager->match_container == nullptr)
				{
					return;
				}

				const auto match = game::s_mgoMatchMakingManager->match_container->match;

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
						const auto slot = &game::s_mgoMatchMakingManager->match_container->match->match_rules.slots[i];
						console::info("matchsetslot %i %s %i\n", i, entry.first.data(), read_field(slot, entry.second));
					}

					console::info("\n");
				}
			});
		}
	};
}

REGISTER_COMPONENT(matchmaking::component)
