#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "scheduler.hpp"
#include "console.hpp"
#include "matchmaking.hpp"
#include "custom_maps.hpp"
#include "map_rotation.hpp"

#include "text_chat/defs.hpp"
#include "text_chat/ui.hpp"

#include "utils/steam.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

#define MATCH_VERSION 170

namespace matchmaking
{
	namespace
	{
		vars::var_ptr var_match_enable_tweaks;
		vars::var_ptr var_match_min_players;
		vars::var_ptr var_match_max_players;
		vars::var_ptr var_match_briefing_time;
		vars::var_ptr var_match_password;
		vars::var_ptr var_match_restricted;

		utils::hook::detour create_lobby_cb_hook;
		utils::hook::detour create_lobby_hook;
		utils::hook::detour join_lobby_cb_hook;
		utils::hook::detour start_transition_hook;
		utils::hook::detour match_start_hook;

		game::match_settings_t match_settings{};

		std::atomic_bool request_match_start = false;
		std::atomic_bool request_match_rotate = false;
		std::atomic_bool request_disconnect = false;

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

		std::array<game::steam_id, 16> kicked_steam_ids;

		struct callback_t
		{
			std::uint32_t type;
			std::function<void(game::mgo_match_t*, const game::steam_id)> callback;
		};

		std::vector<callback_t> event_callbacks;

		game::steam_id last_requested_lobby;

#define DEFINE_MATCH_FIELD(__struct__, __name__) \
		{#__name__, match_field_t(sizeof(__struct__::__name__), offsetof(__struct__, __name__))}

		std::unordered_map<std::string, match_field_t> match_settings_fields =
		{
			DEFINE_MATCH_FIELD(game::match_settings_t, match_type),
			DEFINE_MATCH_FIELD(game::match_settings_t, skill_level),
			DEFINE_MATCH_FIELD(game::match_settings_t, cheat_rate),
			DEFINE_MATCH_FIELD(game::match_settings_t, member_max),
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
			map_rotation::start_rotation(match);
			create_lobby_hook.invoke<void>(match, &match->match_settings);
		}

		void update_match_settings()
		{
			const auto match_container = game::s_mgoMatchMakingManager->match_container; 
			if (match_container == nullptr || match_container->match == nullptr)
			{
				return;
			}

			const auto match = match_container->match;
			if (match_settings.rules.pl_current_match < 0)
			{
				match_settings.rules.pl_current_match = 0;
			}

			if (match_settings.rules.pl_current_match > 4)
			{
				match_settings.rules.pl_current_match = 4;
			}

			match_settings.map_id = match_settings.rules.slots[match_settings.rules.pl_current_match].m_map_id;
			match_settings.match_rule = match_settings.rules.slots[match_settings.rules.pl_current_match].m_match_rule;
			match_settings.walker_gear = match_settings.rules.slots[match_settings.rules.pl_current_match].m_walker_gear;
			match_settings.day_night = match_settings.rules.slots[match_settings.rules.pl_current_match].m_dn;
			match_settings.match_variant = match_settings.rules.slots[match_settings.rules.pl_current_match].m_variant;
			match_settings.unique_char = match_settings.rules.slots[match_settings.rules.pl_current_match].m_unique_char;

			std::memcpy(&match->match_settings, &match_settings, sizeof(game::match_settings_t));
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

		void set_slot_field(int slot_number, const std::string& field, const std::string& value)
		{
			if (field == "m_map_name")
			{
				match_settings.rules.slots[slot_number].m_map_id = custom_maps::get_map_id(value);
			}
			else
			{
				const auto iter = match_slot_fields.find(field);
				if (slot_number >= 5 || iter == match_slot_fields.end())
				{
					console::warn("field \"%s\" does not exist\n", field.data());
					return;
				}

				set_field(&match_settings.rules.slots[slot_number], iter->second, std::atoi(value.data()));
			}

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
				game::s_mgoMatchMakingManager->state = 21;
				request_match_rotate = false;
			}

			if (request_disconnect)
			{
				utils::hook::invoke<void>(SELECT_VALUE_LANG(0x140892850, 0x0), game::s_mgoMatchMakingManager.get(), 1);
			}

			request_disconnect = false;
		}

		void run_callbacks(const std::uint32_t type, game::mgo_match_t* match, const game::steam_id lobby_id)
		{
			const auto match_container = game::s_mgoMatchMakingManager->match_container;
			if (match_container == nullptr || match_container->match == nullptr)
			{
				return;
			}

			for (auto& callback : event_callbacks)
			{
				if (callback.type == type)
				{
					callback.callback(match, lobby_id);
				}
			}
		}

		void update_kick_list()
		{
			const auto match = get_match();
			if (match == nullptr)
			{
				return;
			}

			std::memcpy(match->kicked_ids, kicked_steam_ids.data(), sizeof(game::steam_id) * kicked_steam_ids.size());

			auto count = 0;
			for (auto i = 0ull; i < kicked_steam_ids.size(); i++)
			{
				if (kicked_steam_ids[i].bits != 0)
				{
					++count;
				}

				utils::steam::set_lobby_data(match->lobby_id, "kicked_id", kicked_steam_ids[i].bits, static_cast<int>(i));
			}

			utils::steam::set_lobby_data(match->lobby_id, "kick_num", count);
			match->kick_num = count;
		}

		void update_match_password()
		{
			const auto& password = var_match_password->current.get_string();
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

		void create_lobby_cb_stub(game::mgo_match_t* match, game::steam_id lobby_id)
		{
			console::info("[SteamMatchmaking] Created lobby %llu\n", lobby_id.bits);
			create_lobby_cb_hook.invoke<void>(match, lobby_id);
			run_callbacks(event_create_lobby, match, lobby_id);
		}

		void join_lobby_cb_stub(game::mgo_match_t* match, game::steam_id lobby_id)
		{
			join_lobby_cb_hook.invoke<void>(match, lobby_id);
			run_callbacks(event_join_lobby, match, lobby_id);
		}

		void start_transition_stub()
		{
			start_transition_hook.invoke<void>();

			const auto match_container = game::s_mgoMatchMakingManager->match_container;
			if (match_container == nullptr || match_container->match == nullptr)
			{
				return;
			}

			run_callbacks(event_start_transition, match_container->match, match_container->match->lobby_id);
		}

		char match_start_stub(__int64 a1, __int64 a2, unsigned __int8 a3)
		{
			const auto result = match_start_hook.invoke<char>(a1, a2, a3);

			const auto match_container = game::s_mgoMatchMakingManager->match_container;
			if (match_container == nullptr || match_container->match == nullptr)
			{
				return result;
			}

			run_callbacks(event_match_start, match_container->match, match_container->match->lobby_id);

			return result;
		}

		game::ISteamMatchmaking_vtbl steam_matchmaking_vtbl{};
		bool request_lobby_data_stub(game::ISteamMatchmaking* this_, game::steam_id lobby_id)
		{
			console::debug("[SteamMatchmaking] RequestLobbyData %lli\n", lobby_id.bits);
			last_requested_lobby = lobby_id;
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
			if (var_match_restricted->current.enabled() && key == "version"s)
			{
				value = utils::string::va("%i", MATCH_VERSION);
			}

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

			if (key == "version"s)
			{
				steam_matchmaking_vtbl.AddRequestLobbyListNumericalFilter(this_, key, MATCH_VERSION, -2);
				steam_matchmaking_vtbl.AddRequestLobbyListNumericalFilter(this_, key, value, 2);
			}
			else
			{
				steam_matchmaking_vtbl.AddRequestLobbyListNumericalFilter(this_, key, value, compare);
			}
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

	void ban_player_from_lobby(const game::steam_id steam_id)
	{
		for (auto i = 0; i < kicked_steam_ids.size(); i++)
		{
			if (kicked_steam_ids[i].bits == steam_id.bits)
			{
				return;
			}
		}

		for (auto i = 0; i < kicked_steam_ids.size(); i++)
		{
			if (kicked_steam_ids[i].bits == 0)
			{
				kicked_steam_ids[i].bits = steam_id.bits;
				break;
			}
		}

		update_kick_list();
	}

	void unban_player_from_lobby(const game::steam_id steam_id)
	{
		for (auto i = 0; i < kicked_steam_ids.size(); i++)
		{
			if (kicked_steam_ids[i].bits == steam_id.bits)
			{
				kicked_steam_ids[i].bits = 0;
			}
		}

		update_kick_list();
	}

	void kick_player_from_lobby(const game::steam_id steam_id)
	{
		const auto match_container = game::s_mgoMatchMakingManager->match_container;
		if (match_container == nullptr)
		{
			return;
		}

		kick_msg_t kick_msg{};
		kick_msg.type = 1;
		kick_msg.unk = 0xFFFFFFFF;
		kick_msg.steam_id = steam_id.bits;

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

	void register_callback(const std::uint32_t type, const std::function<void(game::mgo_match_t*, const game::steam_id)> callback)
	{
		callback_t c{};
		c.type = type;
		c.callback = callback;
		event_callbacks.emplace_back(c);
	}

	game::steam_id get_current_steam_id()
	{
		static game::steam_id result{};
		if (result.bits != 0)
		{
			return result;
		}

		const auto steam_user = (*game::SteamUser)();
		if (steam_user == nullptr)
		{
			return {};
		}

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

	game::steam_id get_last_requested_lobby()
	{
		return last_requested_lobby;
	}

	game::mgo_match_t* get_match()
	{
		const auto match_container = game::s_mgoMatchMakingManager->match_container;
		if (match_container == nullptr || match_container->match == nullptr)
		{
			return nullptr;
		}

		return match_container->match;
	}

	game::steam_id get_lobby_id()
	{
		const auto match = get_match();
		if (match == nullptr)
		{
			return {};
		}

		return match->lobby_id;
	}

	bool is_host()
	{
		const auto match = get_match();
		if (match == nullptr)
		{
			return false;
		}

		const auto self_id = get_current_steam_id();
		return match->lobby_owner.bits == self_id.bits;
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

			custom_maps::register_maps();

			var_match_enable_tweaks = vars::register_bool("match_enable_tweaks", false, vars::var_flag_saved, "enable match settings tweaks");
			var_match_min_players = vars::register_int("match_min_players", 2, 0, 16, vars::var_flag_saved, "match minimum players override");
			var_match_max_players = vars::register_int("match_max_players", 16, 0, 16, vars::var_flag_saved, "match maximum players override");
			var_match_briefing_time = vars::register_int("match_briefing_time", 60, 0, 600, vars::var_flag_saved, "match briefing time override (seconds)");
			var_match_password = vars::register_string("match_password", "", vars::var_flag_saved, "match password");
			var_match_restricted = vars::register_bool("match_restricted", false, vars::var_flag_saved, "restrict match to tpp-mod users of compatible versions only");

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

				set_slot_field(params.get_int(1), params.get(2), params.get(3));
			});

			command::add("matchstart", [](const command::params& params)
			{
				request_match_start = true;
			});

			register_callback(event_create_lobby, [](game::mgo_match_t*, game::steam_id)
			{
				update_kick_list();
				update_match_password();
			});
		}

		void start() override
		{
			if (!game::environment::is_mgo())
			{
				return;
			}

			create_lobby_cb_hook.create(SELECT_VALUE_LANG(0x1405A18D0, 0x0), create_lobby_cb_stub);
			create_lobby_hook.create(SELECT_VALUE_LANG(0x1405A1B50, 0x0), create_lobby_stub);
			join_lobby_cb_hook.create(SELECT_VALUE_LANG(0x1405A2F70, 0x0), join_lobby_cb_stub);
			start_transition_hook.create(SELECT_VALUE_LANG(0x1408A23D0, 0x0), start_transition_stub);
			match_start_hook.create(SELECT_VALUE_LANG(0x1408A1F10, 0x0), match_start_stub);

			scheduler::once(hook_steam_matchmaking, scheduler::net);
			scheduler::loop(run_frame, scheduler::session);

			command::add("clearkicks", []()
			{
				std::memset(kicked_steam_ids.data(), 0, kicked_steam_ids.size() * sizeof(game::steam_id));
				update_kick_list();
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
				request_match_rotate = true;
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
