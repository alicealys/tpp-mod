#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "scheduler.hpp"
#include "console.hpp"
#include "vars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace dedicated_server
{
	namespace
	{
		constexpr auto net_channel_max = 0xFFFF;

		utils::hook::detour translate_messages_hook;

		vars::var_ptr var_net_channel;

		game::ISteamNetworking_vtbl steam_networking_vtbl{};

		int current_host_channel{};

		__int64 gn_execute_stub()
		{
			return -1;
		}

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

		game::steam_id get_lobby_id()
		{
			game::steam_id lobby_id{};

			const auto match_container = game::s_MgoMatchmakingManager->match_container;
			if (match_container == nullptr)
			{
				return lobby_id;
			}

			return match_container->match->lobby_id;
		}
		
		int get_var_net_channel()
		{
			return var_net_channel->latched.get_int();
		}

		void set_net_channel(const game::steam_id lobby_id)
		{
			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			const auto value = utils::string::va("%i", get_var_net_channel());
			steam_matchmaking->__vftable->SetLobbyMemberData(steam_matchmaking, lobby_id, "net_channel", value);
		}

		int get_client_net_channel(const game::steam_id lobby_id, const game::steam_id user)
		{
			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			const auto net_channel_value = steam_matchmaking->__vftable->GetLobbyMemberData(steam_matchmaking, lobby_id, user, "net_channel");
			if (net_channel_value == nullptr)
			{
				return 0;
			}

			return std::atoi(net_channel_value);
		}

		bool is_host()
		{
			const auto session = *game::s_pSession;
			if (session != nullptr)
			{
				return session->sessionInterface.__vftable->IsHost(&session->sessionInterface);
			}

			return false;
		}

		bool read_p2p_packet_stub(game::ISteamNetworking* this_, void* pub_dest, unsigned int cub_dest, 
			unsigned int* msg_size, game::steam_id* remote, int /*channel*/)
		{
			return steam_networking_vtbl.ReadP2PPacket(this_, pub_dest, cub_dest, msg_size, remote, get_var_net_channel());
		}

		bool send_p2p_packet_stub(game::ISteamNetworking* this_, game::steam_id remote, 
			void* pub_data, unsigned int cub_data, int type, int /*channel*/)
		{
			const auto channel = get_client_net_channel(get_lobby_id(), remote);
			return steam_networking_vtbl.SendP2PPacket(this_, remote, pub_data, cub_data, type, channel);
		}

		bool is_p2p_packet_available(game::ISteamNetworking* this_, unsigned int* msg_size, int /*channel*/)
		{
			return steam_networking_vtbl.IsP2PPacketAvailable(this_, msg_size, get_var_net_channel());
		}

		void hook_steam_networking()
		{
			const auto steam_networking = (*game::SteamNetworking)();

			steam_networking_vtbl.ReadP2PPacket = steam_networking->__vftable->ReadP2PPacket;
			steam_networking_vtbl.SendP2PPacket = steam_networking->__vftable->SendP2PPacket;
			steam_networking_vtbl.IsP2PPacketAvailable = steam_networking->__vftable->IsP2PPacketAvailable;

			utils::hook::set(&steam_networking->__vftable->ReadP2PPacket, read_p2p_packet_stub);
			utils::hook::set(&steam_networking->__vftable->SendP2PPacket, send_p2p_packet_stub);
			utils::hook::set(&steam_networking->__vftable->IsP2PPacketAvailable, is_p2p_packet_available);
		}

		void set_lobby_data()
		{
			const auto lobby_id = get_lobby_id();
			if (lobby_id.bits == 0)
			{
				return;
			}

			set_net_channel(lobby_id);
		}

		int create_channel_mutex(const int start)
		{
			static HANDLE mutex = NULL;
			atexit([]
			{
				if (mutex == NULL)
				{
					return;
				}

				ReleaseMutex(mutex);
				CloseHandle(mutex);
			});

			for (auto i = start; i < net_channel_max; i++)
			{
				const auto name = utils::string::va("mgo_net_channel_%i", i);
				mutex = CreateMutex(NULL, FALSE, name);
				
				if (mutex == NULL)
				{
					continue;
				}

				if (GetLastError() == ERROR_ALREADY_EXISTS)
				{
					ReleaseMutex(mutex);
					CloseHandle(mutex);
					continue;
				}

				return i;
			}

			return -1;
		}

		void set_net_channel()
		{
			const auto net_channel = create_channel_mutex(get_var_net_channel());
			if (net_channel == -1)
			{
				console::error("failed to set net channel (all channels in use)\n");
				exit(0);
			}

			console::info("[SteamNetworking] Using channel %i\n", net_channel);
			vars::set_var(var_net_channel, net_channel, vars::var_source_internal);
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
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			var_net_channel = vars::register_int("net_channel", 0, 0, 0xFFFF, vars::var_flag_latched, "steam networking channel");
		}

		void start() override
		{
			if (!game::environment::is_mgo() || !game::environment::is_eng())
			{
				return;
			}

			set_net_channel();

			scheduler::once(hook_steam_networking, scheduler::net);
			scheduler::loop(set_lobby_data, scheduler::net, 1s);

			if (!game::environment::is_dedi())
			{
				return;
			}

			SetConsoleTitle("MGO Dedicated Server");
			scheduler::loop(update_console_title, scheduler::net, 1s);

			utils::hook::set<std::uint8_t>(0x140A9F880, 0xC3); // dont build scene
			utils::hook::set<std::uint8_t>(0x149865F0B, 0xEB); // ^
			utils::hook::set<std::uint8_t>(0x14930E490, 0xC3); // dont render scene

			utils::hook::set<std::uint8_t>(0x14A91F2C0, 0xC3); // kill sound
			utils::hook::set<std::uint8_t>(0x140CE5630, 0xC3);
			utils::hook::set<std::uint8_t>(0x146579190, 0xC3);
			utils::hook::set<std::uint8_t>(0x14A91F2C0, 0xC3);
			utils::hook::set<std::uint8_t>(0x140E1A4B0, 0xC3);

			utils::hook::set<std::uint8_t>(0x14A1E39C0, 0xC3);

			utils::hook::jump(0x141451F40, gn_execute_stub);
			utils::hook::nop(0x14258DC10, 5);
			translate_messages_hook.create(0x142590640, translate_messages_stub);

		}
	};
}

REGISTER_COMPONENT(dedicated_server::component)
