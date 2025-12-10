#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "scheduler.hpp"
#include "console.hpp"
#include "command.hpp"
#include "matchmaking.hpp"
#include "vars.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>
#include <utils/cryptography.hpp>
#include <utils/io.hpp>

#include <discord_rpc.h>

namespace discord
{
	namespace
	{
		struct discord_presence_strings_t
		{
			std::string state;
			std::string details;
			std::string small_image_key;
			std::string small_image_text;
			std::string large_image_key;
			std::string large_image_text;
			std::string party_id;
			std::string join_secret;
		};

		DiscordRichPresence discord_presence{};
		discord_presence_strings_t discord_strings;

		vars::var_ptr var_discord_enable;

		enum string_ids : std::uint32_t
		{
			aerial_command_center = 1373758758,
			northern_kabul_afghanistan_1 = 152702784,
			northern_kabul_afghanistan_2 = 583793986,
			northern_kabul_afghanistan_3 = 3438240887,
			northern_kabul_afghanistan_4 = 1077138525,
			angola_zaire_border_region_1 = 1031205248,
			angola_zaire_border_region_2 = 3063837159,
			hospital_mena_de_barranquilla_colombia = 744102497,
			mother_base_seychelles_waters = 2859948209,

			comm_control = 3521205522,
			bounty_hunter_1 = 1715984803,
			bounty_hunter_2 = 4172308629,
			sabotage = 2171042730,
			cloak_and_dagger = 1510345948,
			freeplay = 1965975201,

			jade_forest = 800092068,
			gray_rampart = 3449842512,
			red_fortress = 1475160674,
			black_site = 2649983836,
			amber_station = 2142188143,
			coral_complex = 2899302814,
			azure_mountain = 621877069,
			rust_palace = 492283241,

		};

		template <typename... Args>
		const char* get_localized_string(const char* fmt, Args&&... args)
		{
			game::fox::StringId string_id{};
			const auto key = utils::string::va(fmt, std::forward<Args>(args)...);
			game::tpp::ui::utility::GetStringId(&string_id, key);
			return game::tpp::ui::utility::GetLangText(string_id);
		}

		const char* get_localized_string(const std::uint32_t id)
		{
			game::fox::StringId string_id{};
			string_id.f.l = id;
			return game::tpp::ui::utility::GetLangText(string_id);
		}

		std::string get_map_name(const int map_id)
		{
			static std::unordered_map<int, std::uint32_t> maps =
			{
				{0, jade_forest},
				{1, gray_rampart},
				{2, red_fortress},
				{3, black_site},
				{4, amber_station},
				{5, coral_complex},
				{6, azure_mountain},
				{7, rust_palace},
			};

			const auto iter = maps.find(map_id);
			if (iter == maps.end())
			{
				return "";
			}

			return get_localized_string(iter->second);
		}

		std::string get_gamemode_name(const int match_rule)
		{
			static std::unordered_map<int, std::uint32_t> gamemodes =
			{
				{1, comm_control},
				{2, bounty_hunter_1},
				{3, cloak_and_dagger},
				{4, freeplay},
				{8, sabotage},
			};

			const auto iter = gamemodes.find(match_rule);
			if (iter == gamemodes.end())
			{
				return "";
			}

			return get_localized_string(iter->second);
		}

		void update_discord_mgo()
		{
			discord_strings = {};

			const auto unk = *game::s_unk1;
			const auto session = *game::s_pSession;

			if (session == nullptr || unk == nullptr || unk->match->lobby_id.bits == 0)
			{
				discord_presence.state = "Main Menu";

				discord_presence.startTimestamp = 0;

				discord_presence.partyMax = 0;
				discord_presence.partySize = 0;

				discord_presence.partyId = nullptr;
				discord_presence.joinSecret = nullptr;
				discord_presence.largeImageKey = nullptr;
			}
			else
			{
				discord_presence.state = "In a Match";

				const auto steam_matchmaking = (*game::SteamMatchmaking)();
				const auto max_members = steam_matchmaking->__vftable->GetLobbyMemberLimit(steam_matchmaking, unk->match->lobby_id);

				const auto map_id = std::atoi(steam_matchmaking->__vftable->GetLobbyData(steam_matchmaking, unk->match->lobby_id, "map_id"));
				const auto day_night = std::atoi(steam_matchmaking->__vftable->GetLobbyData(steam_matchmaking, unk->match->lobby_id, "day_night"));
				const auto match_rule = std::atoi(steam_matchmaking->__vftable->GetLobbyData(steam_matchmaking, unk->match->lobby_id, "match_rule"));

				discord_strings.details = std::format("{} - {}", get_gamemode_name(match_rule), get_map_name(map_id));
				discord_strings.large_image_key = utils::string::va("map_%i_%i", map_id, day_night);
				discord_strings.large_image_text = get_map_name(map_id);

				discord_strings.join_secret = utils::string::va("%lli", unk->match->lobby_id.bits);
				discord_strings.party_id = utils::cryptography::sha1::compute(discord_strings.join_secret, true).substr(0, 8);

				discord_presence.partyMax = max_members;
				discord_presence.partySize = session->sessionInterface.__vftable->GetMemberCount(&session->sessionInterface);

				if (discord_presence.startTimestamp == 0)
				{
					discord_presence.startTimestamp = std::chrono::duration_cast<std::chrono::seconds>(
						std::chrono::system_clock::now().time_since_epoch()).count();
				}

				discord_presence.partyId = discord_strings.party_id.data();
				discord_presence.joinSecret = discord_strings.join_secret.data();
				discord_presence.largeImageKey = discord_strings.large_image_key.data();
			}

			discord_presence.details = discord_strings.details.data();
		}

		void update_discord_mgsv()
		{
			discord_presence.largeImageKey = "mgsv";

			const auto location_id = game::tpp::ui::utility::GetCurrentLocationId();
			const auto mission_id = game::tpp::ui::utility::GetCurrentMissionId();

			discord_strings = {};

			if (mission_id != 1 && mission_id != 0xFFFF)
			{
				discord_strings.details = get_localized_string("name_mission_%02d_%05d", location_id, mission_id);

				if (!discord_strings.details.empty() || mission_id == 50050)
				{
					discord_strings.large_image_key = utils::string::va("mission_%i", mission_id);
				}
				else
				{
					discord_strings.details = "Free Roam";
					discord_strings.large_image_key = utils::string::va("location_%i", location_id);
				}

				switch (location_id)
				{
				case 50:
					discord_strings.state = get_localized_string(mother_base_seychelles_waters);
					break;
				case 40:
					discord_strings.state = "";
					break;
				case 30:
					discord_strings.state = get_localized_string(hospital_mena_de_barranquilla_colombia);
					break;
				case 20:
					discord_strings.state = get_localized_string(angola_zaire_border_region_1);
					break;
				case 10:
					discord_strings.state = get_localized_string(northern_kabul_afghanistan_1);
					break;
				}

				if (mission_id == 50050)
				{
					discord_strings.details = "FOB Mission";
					discord_strings.state = "";
				}

				if (mission_id - location_id == 40000)
				{
					discord_strings.details = get_localized_string(aerial_command_center);
				}

				const auto session = *game::s_pSession;
				if (session != nullptr)
				{
					discord_presence.partyMax = session->allMembers.size;
					discord_presence.partySize = session->sessionInterface.__vftable->GetMemberCount(&session->sessionInterface);
					discord_presence.partyPrivacy = DISCORD_PARTY_PRIVATE;
				}
				else
				{
					discord_presence.partyMax = 0;
					discord_presence.partySize = 0;
					discord_presence.partyPrivacy = 0;
				}
			}

			discord_presence.largeImageKey = discord_strings.large_image_key.data();
			discord_presence.state = discord_strings.state.data();
			discord_presence.details = discord_strings.details.data();
		}

		void update_discord()
		{
			Discord_RunCallbacks();

			SELECT_VALUE(update_discord_mgsv, update_discord_mgo)();

			Discord_UpdatePresence(&discord_presence);
		}

		void join_game(const char* join_secret)
		{
			console::debug("Discord: join_game called with secret '%s'\n", join_secret);

			game::steam_id lobby_id{};
			lobby_id.bits = std::strtoull(join_secret, nullptr, 0);

			scheduler::once([=]
			{
				matchmaking::connect_to_lobby(lobby_id);
			}, scheduler::pipeline::main);
		}

		void ready(const DiscordUser* /*request*/)
		{
			std::memset(&discord_presence, 0, sizeof(discord_presence));

			discord_presence.instance = 1;
			discord_presence.state = "";

			Discord_UpdatePresence(&discord_presence);
		}

		void errored(const int error_code, const char* message)
		{
			console::error("Discord: (%i) %s", error_code, message);
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			var_discord_enable = vars::register_bool("discord_enable", true, vars::var_flag_saved | vars::var_flag_latched, "enable discord rpc");
		}

		void start() override
		{
			if (game::environment::is_dedi() || !var_discord_enable->latched.enabled())
			{
				return;
			}

			DiscordEventHandlers handlers{};

			handlers.ready = ready;
			handlers.errored = errored;
			handlers.disconnected = errored;

			if (game::environment::is_mgo())
			{
				handlers.joinGame = join_game;
			}

			handlers.spectateGame = nullptr;
			handlers.joinRequest = nullptr;

			Discord_Initialize(SELECT_VALUE(
				"1104156817845665792", "1416421354365911124", "1104156817845665792", "1416421354365911124"), &handlers, 1, nullptr);

			scheduler::loop(update_discord, scheduler::pipeline::net, 1s);
		}
	};
}

REGISTER_COMPONENT(discord::component)
