#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "scheduler.hpp"
#include "console.hpp"
#include "matchmaking.hpp"

#include "game_log/defs.hpp"
#include "game_log/ui.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace matchmaking
{
	namespace
	{
		utils::hook::detour create_lobby_cb_hook;

		game::match_settings_t match_settings{};

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
				console::warn("field \"%s\" does not exist\n", field.data());
				return;
			}

			set_field(&match_settings.rules.slots[slot_number], iter->second, value);
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
		}

		void update_match_settings()
		{
			if (game::s_mgoMatchMakingManager->match_container == nullptr)
			{
				return;
			}

			std::memcpy(&game::s_mgoMatchMakingManager->match_container->match->match_settings, &match_settings, sizeof(game::match_settings_t));
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

				utils::hook::invoke<void>(SELECT_VALUE_LANG(0x148D00CE0, 0x147DF7DE0));
				game::s_mgoMatchMakingManager->state = 21;
			}

			if (request_disconnect)
			{
				utils::hook::invoke<void>(SELECT_VALUE_LANG(0x140891C80, 0x140891740), game::s_mgoMatchMakingManager.get(), 1);
			}

			request_match_rotate = false;
			request_disconnect = false;
		}

		void set_lobby_data(const std::string& key, const std::string& value)
		{
			const auto match_container = game::s_mgoMatchMakingManager->match_container;
			if (match_container == nullptr)
			{
				return;
			}

			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			const auto res = steam_matchmaking->__vftable->SetLobbyData(steam_matchmaking, match_container->match->lobby_id, key.data(), value.data());
			printf("[SteamMatchmaking] SetLobbyData(%s, %s) = %i\n", key.data(), value.data(), res);
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

		void* create_lobby_cb_stub(void* a1, game::steam_id lobby_id)
		{
			printf("[SteamMatchmaking] Created lobby %llu\n", lobby_id.bits);
			return create_lobby_cb_hook.invoke<void*>(a1, lobby_id);
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
			game_log::reset_log();
			return steam_matchmaking_vtbl.JoinLobby(this_, lobby_id);
		}

		void leave_lobby_stub(game::ISteamMatchmaking* this_, game::steam_id lobby_id)
		{
			console::info("[SteamMatchmaking] LeaveLobby %lli", lobby_id.bits);
			game_log::reset_log();
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
	}

	void ban_player_from_lobby(const std::uint64_t steam_id)
	{
		kicked_steam_ids.insert(steam_id);

		set_lobby_data("kick_num", kicked_steam_ids.size());

		auto index = 0;
		for (const auto& id : kicked_steam_ids)
		{
			set_lobby_data(utils::string::va("kicked_id_%d", index++), id);
		}
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
		void start() override
		{

		}

		void pre_load() override
		{
			if (!game::environment::is_mgo())
			{
				return;
			}

			create_lobby_cb_hook.create(SELECT_VALUE_LANG(0x144EF10B0, 0x1466D0C80), create_lobby_cb_stub);

			scheduler::once(hook_steam_matchmaking, scheduler::net);
			scheduler::loop(run_frame, scheduler::main);

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
