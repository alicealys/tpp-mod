#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "config.hpp"

#include <utils/hook.hpp>
#include <utils/flags.hpp>
#include <utils/concurrency.hpp>
#include <utils/string.hpp>

namespace network
{
	namespace
	{
		struct ip_port_fields
		{
			std::uint8_t ip[4];
			std::uint16_t port;
			std::uint16_t unused;
		};

		struct net_address
		{
			union
			{
				ip_port_fields fields;
				std::uint64_t value;
			};
		};

		static_assert(sizeof(net_address) == sizeof(game::steam_id));

		std::uint16_t socket_port;

		utils::hook::detour nt_daemon_constructor_hook;
		utils::hook::detour cmd_get_fob_target_detail_result_unpack_hook;

		SOCKET create_socket(int port)
		{
			sockaddr_in address{};

			address.sin_family = AF_INET;
			address.sin_port = ntohs(static_cast<short>(port));

			const auto sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

			u_long arg = 1;
			ioctlsocket(sock, FIONBIO, &arg);
			char optval[4] = {1};
			setsockopt(sock, 0xFFFF, 32, optval, 4);

			if (bind(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != -1)
			{
				printf("[socket] bound to port %i\n", port);
				return sock;
			}

			printf("[socket] failed to bind to port %i (%i)\n", port, WSAGetLastError());

			closesocket(sock);
			return 0;
		}

		void set_port(const std::uint32_t port)
		{
			socket_port = static_cast<std::uint16_t>(port);
			utils::hook::set(SELECT_VALUE(0x145970AFD, 0x0) + 3, port);
			utils::hook::set(SELECT_VALUE(0x145970B0B, 0x0) + 3, port);
		}

		SOCKET get_socket()
		{
			static const auto socket = create_socket(socket_port);
			return socket;
		}

		const char* addr_to_string(net_address& ip_port)
		{
			return utils::string::va("%i.%i.%i.%i:%d", ip_port.fields.ip[0], ip_port.fields.ip[1], ip_port.fields.ip[2], ip_port.fields.ip[3], ip_port.fields.port);
		}

		bool send_p2p_packet(game::ISteamNetworking* this_, game::steam_id steam_id, const char* data, unsigned int data_len, int type, int unk)
		{
			net_address address{};
			address.value = steam_id.bits;

			sockaddr_in addr_in{};
			addr_in.sin_family = AF_INET;
			addr_in.sin_addr.S_un.S_un_b.s_b1 = address.fields.ip[0];
			addr_in.sin_addr.S_un.S_un_b.s_b2 = address.fields.ip[1];
			addr_in.sin_addr.S_un.S_un_b.s_b3 = address.fields.ip[2];
			addr_in.sin_addr.S_un.S_un_b.s_b4 = address.fields.ip[3];
			addr_in.sin_port = htons(address.fields.port);

#ifdef DEBUG
			printf("[SteamNetworking] SendP2PPacket: %s (%i)\n", addr_to_string(address), data_len);
#endif

			const auto addr = reinterpret_cast<const sockaddr*>(&addr_in);
			const auto res = sendto(get_socket(), data, data_len, 0, addr, sizeof(sockaddr));

			if (res < 0)
			{
				printf("[socket] error: %i\n", WSAGetLastError());
				return false;
			}

			return true;
		}

		bool read_p2p_packet(game::ISteamNetworking* this_, char* dest, unsigned int dest_len, unsigned int* data_len, game::steam_id* steam_id)
		{
			sockaddr_in from{};
			int from_size = sizeof(from);

			const auto res = recvfrom(get_socket(), dest, dest_len, 0, reinterpret_cast<sockaddr*>(&from), & from_size);
			if (res < 0)
			{
				return false;
			}

			*data_len = res;

			net_address address{};
			address.fields.ip[0] = from.sin_addr.S_un.S_un_b.s_b1;
			address.fields.ip[1] = from.sin_addr.S_un.S_un_b.s_b2;
			address.fields.ip[2] = from.sin_addr.S_un.S_un_b.s_b3;
			address.fields.ip[3] = from.sin_addr.S_un.S_un_b.s_b4;
			address.fields.port = htons(from.sin_port);

#ifdef DEBUG
			printf("[SteamNetworking] ReadP2PPacket: %s\n", addr_to_string(address));
#endif

			steam_id->bits = address.value;

			return true;
		}

		void accept_p2p_session_with_user(game::ISteamNetworking* this_, net_address address)
		{
			printf("[SteamNetworking] accept %s\n", addr_to_string(address));
		}

		void close_p2p_session_with_user(game::ISteamNetworking* this_, net_address address)
		{
			printf("[SteamNetworking] close %s %lli\n", addr_to_string(address), address.value);
		}

		int is_p2p_packet_available(game::ISteamNetworking* this_, unsigned int* message_size, int unk)
		{
			u_long size{};
			ioctlsocket(get_socket(), FIONREAD, &size);
			if (size <= 0)
			{
				return false;
			}

			*message_size = size;

			return true;
		}

		void hook_steam_networking()
		{
			const auto steam_networking = (*game::SteamNetworking)();
			utils::hook::set(&steam_networking->__vftable->SendP2PPacket, send_p2p_packet);
			utils::hook::set(&steam_networking->__vftable->ReadP2PPacket, read_p2p_packet);
			utils::hook::set(&steam_networking->__vftable->AcceptP2PSessionWithUser, accept_p2p_session_with_user);
			utils::hook::set(&steam_networking->__vftable->CloseP2PSessionWithUser, close_p2p_session_with_user);
			utils::hook::set(&steam_networking->__vftable->IsP2PPacketAvailable, is_p2p_packet_available);
		}

		void* nt_daemon_constructor_stub(void* a1)
		{
			get_socket();
			hook_steam_networking();
			return nt_daemon_constructor_hook.invoke<void*>(a1);
		}

		int cmd_get_fob_target_detail_result_unpack_stub(game::tpp::net::CmdGetFobTargetDetailResult* result)
		{
			const auto res = cmd_get_fob_target_detail_result_unpack_hook.invoke<int>(result);

			net_address address{};

			inet_pton(AF_INET, result->ip->buffer, address.fields.ip);
			address.fields.port = static_cast<std::uint16_t>(result->port);
			result->steam_id = address.value;

			return res;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_mgsv())
			{
				return;
			}

			const auto net_udp = config::get<bool>("net_udp");
			if (!net_udp.has_value() || !net_udp.value())
			{
				return;
			}

			const auto net_port = config::get<std::uint16_t>("net_port");
			if (net_port.has_value())
			{
				set_port(net_port.value());
			}

			nt_daemon_constructor_hook.create(SELECT_VALUE(0x1459ACD80, 0x0), nt_daemon_constructor_stub);
			cmd_get_fob_target_detail_result_unpack_hook.create(SELECT_VALUE(0x14081BDC0, 0x0), cmd_get_fob_target_detail_result_unpack_stub);

			utils::hook::nop(SELECT_VALUE(0x14C4006EE, 0x0), 0x6); // SteamAPI_RegisterCallback
			utils::hook::nop(SELECT_VALUE(0x14C40072F, 0x0), 0x6); // ^
		}
	};
}

REGISTER_COMPONENT(network::component)
