#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "fobs.hpp"
#include "vars.hpp"
#include "console.hpp"
#include "scheduler.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/concurrency.hpp>

namespace fobs
{
	namespace
	{
		vars::var_ptr var_fob_security_challenge_mode;

		bool custom_lobbies_enabled()
		{
			return var_fob_security_challenge_mode->current.get_int() == 1;
		}

		void on_lobby_match_list(game::LobbyMatchList_t* match_list);
		void on_lobby_created(game::LobbyCreated_t* lobby_enter);

		class lobby_list_handler : public game::CCallbackBase
		{
		public:
			lobby_list_handler()
			{
				this->m_iCallback = game::LobbyMatchList_t::k_iCallback;
			}

			void Run(void* pvParam) override
			{
				on_lobby_match_list(reinterpret_cast<game::LobbyMatchList_t*>(pvParam));
			}

			void Run(void* pvParam, bool bIOFailure, game::SteamAPICall_t hSteamAPICall) override
			{
				on_lobby_match_list(reinterpret_cast<game::LobbyMatchList_t*>(pvParam));
			}

			int GetCallbackSizeBytes() override
			{
				return 8;
			}
		};

		class lobby_create_handler : public game::CCallbackBase
		{
		public:
			lobby_create_handler()
			{
				this->m_iCallback = game::LobbyCreated_t::k_iCallback;
			}

			void Run(void* pvParam) override
			{
				on_lobby_created(reinterpret_cast<game::LobbyCreated_t*>(pvParam));
			}

			void Run(void* pvParam, bool bIOFailure, game::SteamAPICall_t hSteamAPICall) override
			{
				on_lobby_created(reinterpret_cast<game::LobbyCreated_t*>(pvParam));
			}

			int GetCallbackSizeBytes() override
			{
				return 8;
			}
		};

		struct
		{
			bool initialized;
			game::ISteamMatchmaking* (*steam_matchmaking)();
			game::ISteamMatchmaking* (*register_callback)(game::CCallbackBase*, unsigned __int64);
			game::ISteamMatchmaking* (*register_call_result)(game::CCallbackBase*, unsigned __int64);
			lobby_list_handler lobby_handler{};
			lobby_create_handler lobby_create_handler{};
		} steam_api{};

		struct lobby_t
		{
			struct mother_base_t
			{
				std::uint32_t construct_param;
				std::uint32_t platform_count;
				std::uint32_t mother_base_id;
				std::uint32_t security_rank;
			};

			game::steam_id lobby_id;
			game::steam_id owner_id;
			std::uint32_t player_id;
			std::uint32_t mother_base_num;
			mother_base_t mother_base_param[4];
			std::int32_t name_plate_id;
			std::int32_t league_rank;
			std::int32_t league_grade;
			std::int32_t espionage_rank;
			std::int32_t espionage_grade;
			std::int32_t espionage_point;
			std::int32_t espionage_win;
			std::int32_t espionage_lose;
			std::int32_t staff_count[10];
			game::tpp::mbm::PlayerBasicInfo::Resource total_resource;
		};

		struct state_t
		{
			bool got_lobby_list;
			std::int32_t lobby_count;
			std::vector<lobby_t> lobby_list;
			std::uint64_t lobby_list_request;

			bool requested_lobby_create;
			bool is_lobby_created;
			std::int32_t lobby_create_result;
			game::steam_id own_lobby_id;

			lobby_t own_lobby_info{};
		};

		utils::hook::detour cmd_get_fob_target_list_option_pack_hook;
		utils::hook::detour cmd_get_fob_target_list_result_unpack_hook;

		utils::hook::detour cmd_set_security_challenge_option_pack_hook;
		utils::hook::detour cmd_set_security_challenge_result_unpack_hook;

		utils::hook::detour cmd_get_playerlist_result_unpack_hook;
		utils::hook::detour cmd_set_currentplayer_result_unpack_hook;
		utils::hook::detour cmd_sync_mother_base_option_pack_hook;
		utils::hook::detour cmd_sync_soldier_bin_pack_hook;
		utils::hook::detour cmd_sync_resource_result_unpack_hook;
		utils::hook::detour cmd_get_own_fob_list_result_unpack_hook;

		utils::hook::detour fob_target_receive_enemy_basic_info_hook;

		utils::concurrency::container<state_t> state;
		
		void update_lobby(state_t& s)
		{
			const auto steam_matchmaking = steam_api.steam_matchmaking();
			if (s.own_lobby_id.bits == 0)
			{
				return;
			}

			const auto set_lobby_data = [&](const char* key, const int index, const int value)
			{
				const auto value_str = utils::string::va("%i", value);

				if (index != -1)
				{
					key = utils::string::va("%s_%i", key, index);
				}

				steam_matchmaking->__vftable->SetLobbyData(steam_matchmaking, s.own_lobby_id, key, value_str);
			};

			const auto owner_id_str = utils::string::va("%llu", s.own_lobby_info.owner_id.bits);
			steam_matchmaking->__vftable->SetLobbyData(steam_matchmaking, s.own_lobby_id, "owner_id", owner_id_str);

			set_lobby_data("is_security_challenge", -1, 1);
			set_lobby_data("player_id", -1, s.own_lobby_info.player_id);
			set_lobby_data("league_rank", -1, s.own_lobby_info.league_rank);
			set_lobby_data("league_grade", -1, s.own_lobby_info.league_grade);
			set_lobby_data("espionage_rank", -1, s.own_lobby_info.espionage_rank);
			set_lobby_data("espionage_grade", -1, s.own_lobby_info.espionage_grade);
			set_lobby_data("espionage_point", -1, s.own_lobby_info.espionage_point);
			set_lobby_data("espionage_win", -1, s.own_lobby_info.espionage_win);
			set_lobby_data("espionage_lose", -1, s.own_lobby_info.espionage_lose);
			set_lobby_data("mother_base_num", -1, s.own_lobby_info.mother_base_num);

			for (auto i = 0; i < 10; i++)
			{
				set_lobby_data("staff_count", i, s.own_lobby_info.staff_count[i]);
			}

			set_lobby_data("fuel_resource", -1, s.own_lobby_info.total_resource.fuel_resource);
			set_lobby_data("biotic_resource", -1, s.own_lobby_info.total_resource.biotic_resource);
			set_lobby_data("common_metal", -1, s.own_lobby_info.total_resource.common_metal);
			set_lobby_data("minor_metal", -1, s.own_lobby_info.total_resource.minor_metal);
			set_lobby_data("precious_metal", -1, s.own_lobby_info.total_resource.precious_metal);

			set_lobby_data("name_plate_id", -1, s.own_lobby_info.name_plate_id);

			for (auto i = 0u; i < s.own_lobby_info.mother_base_num; i++)
			{
				set_lobby_data("construct_param", i, s.own_lobby_info.mother_base_param[i].construct_param);
				set_lobby_data("security_rank", i, s.own_lobby_info.mother_base_param[i].security_rank);
				set_lobby_data("mother_base_id", i, s.own_lobby_info.mother_base_param[i].mother_base_id);
				set_lobby_data("platform_count", i, s.own_lobby_info.mother_base_param[i].platform_count);
			}
		}

		void load_lobbies(state_t& s, int num_lobbies)
		{
			s.lobby_list.clear();

			const auto steam_matchmaking = steam_api.steam_matchmaking();
			const auto add_lobby = [&](game::steam_id lobby_id)
			{
				lobby_t lobby{};

				const auto get_lobby_data = [&](const char* key, const int index = -1)
				{
					if (index != -1)
					{
						key = utils::string::va("%s_%i", key, index);
					}

					return std::atoi(steam_matchmaking->__vftable->GetLobbyData(steam_matchmaking,
						lobby_id, key));
				};

				const auto owner_id = steam_matchmaking->__vftable->GetLobbyData(steam_matchmaking, lobby_id, "owner_id");

				lobby.owner_id.bits = std::strtoull(owner_id, nullptr, 0);
				lobby.player_id = get_lobby_data("player_id");
				lobby.league_rank = get_lobby_data("league_rank");
				lobby.league_grade = get_lobby_data("league_grade");
				lobby.espionage_rank = get_lobby_data("espionage_rank");
				lobby.espionage_grade = get_lobby_data("espionage_grade");
				lobby.espionage_point = get_lobby_data("espionage_point");
				lobby.espionage_win = get_lobby_data("espionage_win");
				lobby.espionage_lose = get_lobby_data("espionage_lose");

				for (auto i = 0; i < 10; i++)
				{
					lobby.staff_count[i] = get_lobby_data("staff_count", i);
				}

				lobby.total_resource.fuel_resource = get_lobby_data("fuel_resource");
				lobby.total_resource.biotic_resource = get_lobby_data("biotic_resource");
				lobby.total_resource.common_metal = get_lobby_data("common_metal");
				lobby.total_resource.minor_metal = get_lobby_data("minor_metal");
				lobby.total_resource.precious_metal = get_lobby_data("precious_metal");

				lobby.name_plate_id = get_lobby_data("name_plate_id");

				lobby.mother_base_num = get_lobby_data("mother_base_num");

				const auto match_type = std::atoi(steam_matchmaking->__vftable->GetLobbyData(steam_matchmaking, lobby_id, "match_type"));

				for (auto o = 0u; o < lobby.mother_base_num; o++)
				{
					lobby.mother_base_param[o].construct_param = get_lobby_data("construct_param", o);
					lobby.mother_base_param[o].security_rank = get_lobby_data("security_rank", o);
					lobby.mother_base_param[o].mother_base_id = get_lobby_data("mother_base_id", o);
					lobby.mother_base_param[o].platform_count = get_lobby_data("platform_count", o);
				}

				s.lobby_list.emplace_back(lobby);
			};

			s.lobby_list.emplace_back(s.own_lobby_info);

			for (auto i = 0; i < num_lobbies; i++)
			{
				game::steam_id lobby_id{};
				steam_matchmaking->__vftable->GetLobbyByIndex(steam_matchmaking, &lobby_id, i);

				if (lobby_id.bits == s.own_lobby_id.bits)
				{
					continue;
				}

				add_lobby(lobby_id);
			}
		}

		void on_lobby_match_list(game::LobbyMatchList_t* match_list)
		{
			state.access([&](state_t& s)
			{
				load_lobbies(s, match_list->num_lobbies);
				s.got_lobby_list = true;
			});
		}

		void on_lobby_created(game::LobbyCreated_t* lobby_enter)
		{
			state.access([&](state_t& s)
			{
				s.lobby_create_result = lobby_enter->result == 1;
				s.own_lobby_id = lobby_enter->lobby_id;
				s.requested_lobby_create = false;
				update_lobby(s);
			});
		}

		void request_create_lobby(state_t& s)
		{
			if (!steam_api.initialized)
			{
				return;
			}

			const auto matchmaking = (*steam_api.steam_matchmaking)();
			const auto handle = matchmaking->__vftable->CreateLobby(matchmaking, 2, 2);

			s.own_lobby_id.bits = 0;
			s.requested_lobby_create = true;
			s.lobby_create_result = -1;
		}

		void request_close_lobby(state_t& s)
		{
			if (!steam_api.initialized)
			{
				return;
			}

			const auto matchmaking = (*steam_api.steam_matchmaking)();
			game::steam_id lobby_id{};
			matchmaking->__vftable->LeaveLobby(matchmaking, s.own_lobby_id);
			s.own_lobby_id.bits = 0;
		}

		void request_lobby_list(state_t& s)
		{
			if (!steam_api.initialized)
			{
				return;
			}

			const auto matchmaking = (*steam_api.steam_matchmaking)();
			matchmaking->__vftable->AddRequestLobbyListStringFilter(matchmaking, "is_security_challenge", "1", 0);
			const auto handle = matchmaking->__vftable->RequestLobbyList(matchmaking);

			s.got_lobby_list = false;
			s.lobby_list.clear();

			steam_api.register_call_result(&steam_api.lobby_handler, handle);
		}

		void wait_for_lobby_list()
		{
			const auto should_wait = [&]()
			{
				return state.access<bool>([&](state_t& s)
				{
					return !s.got_lobby_list;
				});
			};

			const auto start = std::chrono::steady_clock::now();
			while (should_wait() && std::chrono::steady_clock::now() - start < 5s)
			{
				std::this_thread::sleep_for(100ms);
			}
		}

		char cmd_get_fob_target_list_option_pack_stub(game::tpp::net::CmdGetFobTargetListOption* option)
		{
			if (custom_lobbies_enabled() && option->type.data->buffer == "CHALLENGE"s)
			{
				state.access([&](state_t& s)
				{
					request_lobby_list(s);
				});
			}

			return cmd_get_fob_target_list_option_pack_hook.invoke<char>(option);
		}

		char cmd_get_fob_target_list_result_unpack_stub(game::tpp::net::CmdGetFobTargetListResult<16> *list)
		{
			const auto res = cmd_get_fob_target_list_result_unpack_hook.invoke<char>(list);

			if (custom_lobbies_enabled())
			{
				state.access([&](state_t& s)
				{
					list->enable_security_challenge = s.own_lobby_id.bits != 0;
				});

				if (list->type.data->buffer == "CHALLENGE"s)
				{
					wait_for_lobby_list();
				}
			}

			return res;
		}

		char cmd_set_security_challenge_option_pack_stub(game::tpp::net::CmdSetSecurityChallengeOption* option)
		{
			const auto prev_buffer = option->status.data->buffer;

			if (custom_lobbies_enabled())
			{
				option->status.data->buffer = "DISABLE";

				state.access([&](state_t& s)
				{
					if (s.own_lobby_id.bits != 0)
					{
						request_close_lobby(s);
					}
					else
					{
						request_create_lobby(s);
					}
				});
			}

			const auto res = cmd_set_security_challenge_option_pack_hook.invoke<char>(option);
			option->status.data->buffer = prev_buffer;
			return res;
		}

		char cmd_set_security_challenge_result_unpack_stub(game::tpp::net::CmdSetSecurityChallengeResult<16>* result)
		{
			if (custom_lobbies_enabled())
			{
				const auto start = std::chrono::steady_clock::now();
				const auto should_wait = [&]()
				{
					return state.access<bool>([&](state_t& s)
					{
						return s.requested_lobby_create && s.own_lobby_id.bits == 0;
					});
				};

				while (should_wait() && (std::chrono::steady_clock::now() - start) < 5s)
				{
					std::this_thread::sleep_for(100ms);
				}
			}

			return cmd_set_security_challenge_result_unpack_hook.invoke<char>(result);
		}

		char cmd_get_playerlist_result_unpack_stub(game::tpp::net::CmdGetPlayerlistResult<16>* result)
		{
			const auto res = cmd_get_playerlist_result_unpack_hook.invoke<char>(result);
			const auto steam_user = (*game::SteamUser)();

			state.access([&](state_t& s)
			{
				steam_user->__vftable->GetSteamID(steam_user, &s.own_lobby_info.owner_id);
				s.own_lobby_info.league_rank = result->player_data[0].league_rank;
				s.own_lobby_info.league_grade = result->player_data[0].league_grade;
				s.own_lobby_info.espionage_rank = result->player_data[0].fob_rank;
				s.own_lobby_info.espionage_grade = result->player_data[0].fob_grade;
				s.own_lobby_info.espionage_point = result->player_data[0].fob_point;
				s.own_lobby_info.espionage_win = result->player_data[0].espionage_win;
				s.own_lobby_info.espionage_lose = result->player_data[0].espionage_lose;
			});

			return res;
		}

		char cmd_set_currentplayer_result_unpack_stub(game::tpp::net::CmdSetCurrentplayerResult<16> *result)
		{
			const auto res = cmd_set_currentplayer_result_unpack_hook.invoke<char>(result);
			
			state.access([&](state_t& s)
			{
				s.own_lobby_info.player_id = result->player_id;
			});

			return res;
		}

		char cmd_sync_mother_base_option_pack_stub(game::tpp::net::CmdSyncMotherBaseOption* option)
		{
			state.access([&](state_t& s)
			{
				s.own_lobby_info.mother_base_num = option->mother_base_num;
				for (auto i = 0; i < option->mother_base_num; i++)
				{
					s.own_lobby_info.mother_base_param[i].construct_param = option->mother_base_param[i].construct_param;
					s.own_lobby_info.mother_base_param[i].security_rank = option->mother_base_param[i].security_rank;
					s.own_lobby_info.mother_base_param[i].platform_count = option->mother_base_param[i].platform_count;
				}

				s.own_lobby_info.name_plate_id = option->name_plate_id;
			});

			return cmd_sync_mother_base_option_pack_hook.invoke<char>(option);
		}

		char cmd_sync_soldier_bin_pack_stub(game::tpp::net::CmdSyncSoldierBinOption* option)
		{
			state.access([&](state_t& s)
			{
				for (auto i = 0; i < option->soldier_num; i++)
				{
					auto staff = option->soldier_param[i];
					staff.fields.header.data = _byteswap_ulong(staff.fields.header.data);
					s.own_lobby_info.staff_count[staff.fields.header.fields.peak_rank]++;
				}
			});

			return cmd_sync_soldier_bin_pack_hook.invoke<char>(option);
		}

		char cmd_get_own_fob_list_result_unpack_stub(game::tpp::net::CmdGetOwnFobListResult<16>* result)
		{
			const auto res = cmd_get_own_fob_list_result_unpack_hook.invoke<char>(result);

			state.access([&](state_t& s)
			{
				for (auto i = 0; i < result->fob_count; i++)
				{
					s.own_lobby_info.mother_base_param[i].mother_base_id = result->fob[i].mother_base_id;
				}
			});

			return res;
		}

		char cmd_sync_resource_result_unpack_stub(game::tpp::net::CmdSyncResourceResult<16>* result)
		{
			const auto res = cmd_sync_resource_result_unpack_hook.invoke<char>(result);

			state.access([&](state_t& s)
			{
				s.own_lobby_info.total_resource.fuel_resource += result->diff_resource1[0];
				s.own_lobby_info.total_resource.biotic_resource += result->diff_resource1[1];
				s.own_lobby_info.total_resource.common_metal += result->diff_resource1[2];
				s.own_lobby_info.total_resource.minor_metal += result->diff_resource1[3];
				s.own_lobby_info.total_resource.precious_metal += result->diff_resource1[4];

				s.own_lobby_info.total_resource.fuel_resource += result->diff_resource2[0];
				s.own_lobby_info.total_resource.biotic_resource += result->diff_resource2[1];
				s.own_lobby_info.total_resource.common_metal += result->diff_resource2[2];
				s.own_lobby_info.total_resource.minor_metal += result->diff_resource2[3];
				s.own_lobby_info.total_resource.precious_metal += result->diff_resource2[4];

				s.own_lobby_info.total_resource.fuel_resource += result->fix_resource1[0];
				s.own_lobby_info.total_resource.biotic_resource += result->fix_resource1[1];
				s.own_lobby_info.total_resource.common_metal += result->fix_resource1[2];
				s.own_lobby_info.total_resource.minor_metal += result->fix_resource1[3];
				s.own_lobby_info.total_resource.precious_metal += result->fix_resource1[4];

				s.own_lobby_info.total_resource.fuel_resource += result->fix_resource2[0];
				s.own_lobby_info.total_resource.biotic_resource += result->fix_resource2[1];
				s.own_lobby_info.total_resource.common_metal += result->fix_resource2[2];
				s.own_lobby_info.total_resource.minor_metal += result->fix_resource2[3];
				s.own_lobby_info.total_resource.precious_metal += result->fix_resource2[4];
			});

			return res;
		}

		void fob_target_receive_enemy_basic_info_stub(game::tpp::net::FobTarget* fob_target, game::tpp::net::CmdGetFobTargetListResult<0>* list)
		{
			if (!custom_lobbies_enabled() || list->type.data->buffer != "CHALLENGE"s)
			{
				fob_target_receive_enemy_basic_info_hook.invoke<void>(fob_target, list);
				return;
			}

			game::tpp::net::DisplayName_::ClearList(fob_target->displayName1);
			game::tpp::net::DisplayName_::ClearList(fob_target->displayName2);

			state.access([&](state_t& s)
			{
				for (auto i = 0u; i < s.lobby_list.size(); i++)
				{
					fob_target->playerInfos[i].owner_account.id = s.lobby_list[i].owner_id.bits;
					fob_target->playerInfos[i].mother_base_num = static_cast<char>(s.lobby_list[i].mother_base_num) + 1;
					fob_target->playerInfos[i].owner_player_id = s.lobby_list[i].player_id;

					for (auto o = 0u; o < s.lobby_list[i].mother_base_num; o++)
					{
						fob_target->playerInfos[i].mother_base_id[o] = s.lobby_list[i].mother_base_param[o].mother_base_id;
						fob_target->playerInfos[i].area_id[o] = static_cast<char>((s.lobby_list[i].mother_base_param[o].construct_param >> 1) & 0x7F);
						fob_target->playerInfos[i].construct_param2[o] = static_cast<char>((s.lobby_list[i].mother_base_param[o].construct_param >> 22) & 0x3F);
						fob_target->playerInfos[i].security_rank[o] = static_cast<char>(s.lobby_list[i].mother_base_param[o].security_rank);
						fob_target->playerInfos[i].platform_count[o] = static_cast<char>(s.lobby_list[i].mother_base_param[o].platform_count);
					}

					fob_target->playerInfos[i].owner_ugc = 1;
					fob_target->playerInfos[i].league_rank_grade = static_cast<char>(s.lobby_list[i].league_grade);
					fob_target->playerInfos[i].league_rank_rank = s.lobby_list[i].league_rank;
					fob_target->playerInfos[i].sneak_rank_grade = static_cast<char>(s.lobby_list[i].espionage_grade);
					fob_target->playerInfos[i].sneak_rank_rank = s.lobby_list[i].espionage_rank;
					fob_target->playerInfos[i].espionage_score = s.lobby_list[i].espionage_point;
					fob_target->playerInfos[i].espionage_win = static_cast<short>(s.lobby_list[i].espionage_win);
					fob_target->playerInfos[i].espionage_total = static_cast<short>(s.lobby_list[i].espionage_win + s.lobby_list[i].espionage_lose);

					for (auto o = 0; o < 10; o++)
					{
						fob_target->playerInfos[i].staff_count[o] = static_cast<short>(s.lobby_list[i].staff_count[o]);
						fob_target->playerInfos[i].staff_num += fob_target->playerInfos[i].staff_count[o];
					}

					fob_target->playerInfos[i].usable_resource.fuel_resource = s.lobby_list[i].total_resource.fuel_resource;
					fob_target->playerInfos[i].usable_resource.biotic_resource = s.lobby_list[i].total_resource.biotic_resource;
					fob_target->playerInfos[i].usable_resource.common_metal = s.lobby_list[i].total_resource.common_metal;
					fob_target->playerInfos[i].usable_resource.minor_metal = s.lobby_list[i].total_resource.minor_metal;
					fob_target->playerInfos[i].usable_resource.precious_metal = s.lobby_list[i].total_resource.precious_metal;

					fob_target->playerInfos[i].nameplate_id = static_cast<char>(s.lobby_list[i].name_plate_id);

					game::tpp::net::DisplayName_::AddList(fob_target->displayName1, &fob_target->playerInfos[i].owner_account);
				}
			});

			game::tpp::net::DisplayName_::GetDisplayName(fob_target->displayName1);
			game::tpp::net::DisplayName_::GetDisplayName(fob_target->displayName2);
		}

		void initialize_steam()
		{
			if (steam_api.initialized)
			{
				return;
			}

			const auto steam = utils::nt::library("steam_api64.dll");

			steam_api.steam_matchmaking = steam.get_proc<decltype(steam_api.steam_matchmaking)>("SteamMatchmaking");
			steam_api.register_callback = steam.get_proc<decltype(steam_api.register_callback)>("SteamAPI_RegisterCallback");
			steam_api.register_call_result = steam.get_proc<decltype(steam_api.register_call_result)>("SteamAPI_RegisterCallResult");
			steam_api.register_callback(&steam_api.lobby_create_handler, game::LobbyCreated_t::k_iCallback);

			steam_api.initialized = true;
		}

		void run_frame()
		{
			initialize_steam();
			if (var_fob_security_challenge_mode->changed)
			{
				state.access([&](state_t& s)
				{
					if (s.own_lobby_id.bits != 0)
					{
						request_close_lobby(s);
					}
				});

				var_fob_security_challenge_mode->changed = false;
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			if (!game::environment::is_tpp())
			{
				return;
			}

			var_fob_security_challenge_mode = vars::register_int("fob_security_challenge_mode", 0, 0, 1,
				vars::var_flag_saved, "security challenge mode (0 = konami, 1 = steam lobbies)");
		}

		void start() override
		{
			if (!game::environment::is_tpp())
			{
				return;
			}

			cmd_get_fob_target_list_result_unpack_hook.create(SELECT_VALUE_LANG(0x140816F40, 0x140816B90), cmd_get_fob_target_list_result_unpack_stub);
			cmd_get_fob_target_list_option_pack_hook.create(SELECT_VALUE_LANG(0x145B1AE00, 0x1474F31B0), cmd_get_fob_target_list_option_pack_stub);

			cmd_set_security_challenge_option_pack_hook.create(SELECT_VALUE_LANG(0x145BAE010, 0x14758B7D0), cmd_set_security_challenge_option_pack_stub);
			cmd_set_security_challenge_result_unpack_hook.create(SELECT_VALUE_LANG(0x145B22420, 0x1474FB2E0), cmd_set_security_challenge_result_unpack_stub);

			cmd_get_playerlist_result_unpack_hook.create(SELECT_VALUE_LANG(0x1407E2820, 0x1407E2450), cmd_get_playerlist_result_unpack_stub);
			cmd_set_currentplayer_result_unpack_hook.create(SELECT_VALUE_LANG(0x1459CDBC0, 0x147414460), cmd_set_currentplayer_result_unpack_stub);
			cmd_sync_mother_base_option_pack_hook.create(SELECT_VALUE_LANG(0x145B05460, 0x1474DA240), cmd_sync_mother_base_option_pack_stub);
			cmd_get_own_fob_list_result_unpack_hook.create(SELECT_VALUE_LANG(0x140845C80, 0x1408458B0), cmd_get_own_fob_list_result_unpack_stub);
			cmd_sync_soldier_bin_pack_hook.create(SELECT_VALUE_LANG(0x145B0B710, 0x1474E11F0), cmd_sync_soldier_bin_pack_stub);
			cmd_sync_resource_result_unpack_hook.create(SELECT_VALUE_LANG(0x145B09E50, 0x1474DEB10), cmd_sync_resource_result_unpack_stub);

			fob_target_receive_enemy_basic_info_hook.create(SELECT_VALUE_LANG(0x1459F5940, 0x147443580), fob_target_receive_enemy_basic_info_stub);

			scheduler::loop(run_frame, scheduler::net);
		}

		void end() override
		{
			state.access([&](state_t& s)
			{
				if (s.own_lobby_id.bits != 0)
				{
					request_close_lobby(s);
				}
			});
		}
	};
}

REGISTER_COMPONENT(fobs::component)
