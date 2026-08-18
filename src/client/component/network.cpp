#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "scheduler.hpp"
#include "console.hpp"
#include "vars.hpp"
#include "matchmaking.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/concurrency.hpp>

namespace network
{
	namespace
	{
		constexpr auto net_channel_max = 0xFFFF;

		vars::var_ptr var_net_channel;
		vars::var_ptr var_whitelist;

		game::ISteamNetworking_vtbl steam_networking_vtbl{};

		int current_host_channel{};

		struct player_lists_t
		{
			std::unordered_set<std::uint64_t> banned;
			std::unordered_set<std::uint64_t> whitelisted;
		};

		utils::concurrency::container<player_lists_t> player_lists;

		void send_net_channel(const game::steam_id lobby_id)
		{
			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			const auto value = utils::string::va("%i", var_net_channel->current.get_int());
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

		bool read_p2p_packet_stub(game::ISteamNetworking* this_, void* pub_dest, unsigned int cub_dest, 
			unsigned int* msg_size, game::steam_id* remote, int /*channel*/)
		{
			return steam_networking_vtbl.ReadP2PPacket(this_, pub_dest, cub_dest, msg_size, remote, var_net_channel->current.get_int());
		}

		bool send_p2p_packet_stub(game::ISteamNetworking* this_, game::steam_id remote, 
			void* pub_data, unsigned int cub_data, int type, int /*channel*/)
		{
			const auto channel = get_client_net_channel(matchmaking::get_lobby_id(), remote);
			return steam_networking_vtbl.SendP2PPacket(this_, remote, pub_data, cub_data, type, channel);
		}

		bool is_p2p_packet_available(game::ISteamNetworking* this_, unsigned int* msg_size, int /*channel*/)
		{
			return steam_networking_vtbl.IsP2PPacketAvailable(this_, msg_size, var_net_channel->current.get_int());
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
			const auto net_channel = create_channel_mutex(var_net_channel->current.get_int());
			if (net_channel == -1)
			{
				console::error("[SteamNetworking] failed to set net channel (all channels in use)\n");
				std::exit(0);
			}

			console::info("[SteamNetworking] Using channel %i\n", net_channel);
			vars::set_var(var_net_channel, net_channel, vars::var_source_internal);
		}

		utils::hook::detour on_p2p_session_request_hook;
		void on_p2p_session_request_stub(void* a1, game::P2PSessionRequest_t* request)
		{
			const auto blocked = player_lists.access<bool>([&](player_lists_t& lists)
			{
				if (lists.banned.contains(request->remote.bits))
				{
					return true;
				}

				if (var_whitelist->current.enabled() && !lists.whitelisted.contains(request->remote.bits))
				{
					return true;
				}

				return false;
			});

			if (blocked)
			{
				console::info("[network] not allowing connection from %lli\n", request->remote.bits);
				matchmaking::kick_player_from_lobby(request->remote);
				return;
			}

			console::info("[network] allowing connection from %lli\n", request->remote.bits);
			on_p2p_session_request_hook.invoke<void>(a1, request);
		}

		void on_lobby_join(game::mgo_match_t*, game::steam_id lobby_id)
		{
			send_net_channel(lobby_id);
		}
	}

	void unban_player(const game::steam_id steam_id)
	{
		player_lists.access([&](player_lists_t& lists)
		{
			lists.banned.erase(steam_id.bits);
		});
	}

	void ban_player(const game::steam_id steam_id)
	{
		player_lists.access([&](player_lists_t& lists)
		{
			const auto lobby_id = matchmaking::get_lobby_id();

			if (lobby_id.bits != 0)
			{
				const auto steam_networking = (*game::SteamNetworking)();
				const auto channel = get_client_net_channel(lobby_id, steam_id);
				steam_networking->__vftable->CloseP2PChannelWithUser(steam_networking, steam_id, channel);
			}

			lists.banned.insert(steam_id.bits);
		});
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

			var_net_channel = vars::register_int("net_channel", 0, 0, net_channel_max, vars::var_flag_latched, "steam networking channel");
			var_whitelist = vars::register_bool("whitelist_enable", false, vars::var_flag_saved, "enable whitelist");

			on_p2p_session_request_hook.create(SELECT_VALUE_LANG(0x1414B8FD0, 0x0), on_p2p_session_request_stub);

			command::add("whitelist_add", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}
				
				const auto steam_id = params.get_uint64(1);
				player_lists.access([&](player_lists_t& lists)
				{
					lists.whitelisted.insert(steam_id);
				});
			});

			command::add("whitelist_remove", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto steam_id = params.get_uint64(1);
				player_lists.access([&](player_lists_t& lists)
				{
					lists.whitelisted.erase(steam_id);
				});
			});
		}

		void start() override
		{
			if (!game::environment::is_mgo())
			{
				return;
			}

			set_net_channel();

			scheduler::once(hook_steam_networking, scheduler::net);

			matchmaking::register_callback(matchmaking::event_create_lobby, on_lobby_join);
			matchmaking::register_callback(matchmaking::event_join_lobby, on_lobby_join);
		}
	};
}

REGISTER_COMPONENT(network::component)
