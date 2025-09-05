#pragma once
#include <d3d11.h>

namespace game
{
	// https://github.com/rlabrecque/SteamworksSDK/tree/f4d127e335a66a1615c0870830d14dcee76ed8aa/public/steam

	struct raw_steam_id final
	{
		unsigned int account_id : 32;
		unsigned int account_instance : 20;
		unsigned int account_type : 4;
		int universe : 8;
	};

	typedef union
	{
		raw_steam_id raw;
		unsigned long long bits;
	} steam_id;

	struct ISteamNetworking;

	struct /*VFT*/ ISteamNetworking_vtbl
	{
		bool(__fastcall* SendP2PPacket)(ISteamNetworking* this_, steam_id steamIDRemote, const void* pubData, unsigned int cubData, int eP2PSendType);
		bool(__fastcall* IsP2PPacketAvailable)(ISteamNetworking* this_, unsigned int* pcubMsgSize, int idk);
		bool(__fastcall* ReadP2PPacket)(ISteamNetworking* this_, void* pubDest, unsigned int cubDest, unsigned int* pcubMsgSize, steam_id* psteamIDRemote);
		bool(__fastcall* AcceptP2PSessionWithUser)(ISteamNetworking* this_, steam_id steamIDRemote);
		bool(__fastcall* CloseP2PSessionWithUser)(ISteamNetworking* this_, steam_id steamIDRemote);
		bool(__fastcall* CloseP2PChannelWithUser)(ISteamNetworking* this_, steam_id steamIDRemote, int iVirtualPort);
		bool(__fastcall* GetP2PSessionState)(ISteamNetworking* this_, steam_id steamIDRemote, void* pConnectionState);
		bool(__fastcall* AllowP2PPacketRelay)(ISteamNetworking* this_, bool bAllow);
		unsigned int(__fastcall* CreateListenSocket)(ISteamNetworking* this_, int nVirtualP2PPort, unsigned int nIP, unsigned __int16 nPort, bool bAllowUseOfPacketRelay);
		unsigned int(__fastcall* CreateP2PConnectionSocket)(ISteamNetworking* this_, steam_id steamIDTarget, int nVirtualPort, int nTimeoutSec, bool bAllowUseOfPacketRelay);
		unsigned int(__fastcall* CreateConnectionSocket)(ISteamNetworking* this_, unsigned int nIP, unsigned __int16 nPort, int nTimeoutSec);
		bool(__fastcall* DestroySocket)(ISteamNetworking* this_, unsigned int hSocket, bool bNotifyRemoteEnd);
		bool(__fastcall* DestroyListenSocket)(ISteamNetworking* this_, unsigned int hSocket, bool bNotifyRemoteEnd);
		bool(__fastcall* SendDataOnSocket)(ISteamNetworking* this_, unsigned int hSocket, void* pubData, unsigned int cubData, bool bReliable);
		bool(__fastcall* IsDataAvailableOnSocket)(ISteamNetworking* this_, unsigned int hSocket, unsigned int* pcubMsgSize);
		bool(__fastcall* RetrieveDataFromSocket)(ISteamNetworking* this_, unsigned int hSocket, void* pubDest, unsigned int cubDest, unsigned int* pcubMsgSize);
		bool(__fastcall* IsDataAvailable)(ISteamNetworking* this_, unsigned int hListenSocket, unsigned int* pcubMsgSize, unsigned int* phSocket);
		bool(__fastcall* RetrieveData)(ISteamNetworking* this_, unsigned int hListenSocket, void* pubDest, unsigned int cubDest, unsigned int* pcubMsgSize, unsigned int* phSocket);
		bool(__fastcall* GetSocketInfo)(ISteamNetworking* this_, unsigned int hSocket, steam_id* pSteamIDRemote, int* peSocketStatus, unsigned int* punIPRemote, unsigned __int16* punPortRemote);
		bool(__fastcall* GetListenSocketInfo)(ISteamNetworking* this_, unsigned int hListenSocket, unsigned int* pnIP, unsigned __int16* pnPort);
		int(__fastcall* GetSocketConnectionType)(ISteamNetworking* this_, unsigned int hSocket);
		int(__fastcall* GetMaxPacketSize)(ISteamNetworking* this_, unsigned int hSocket);
	};

	struct ISteamNetworking
	{
		ISteamNetworking_vtbl* __vftable /*VFT*/;
	};

	namespace fox::nio
	{
		struct Address;

		struct IpPortAddress
		{
			char __pad0[40];
		};

		struct Socket
		{
			struct Info;
		};

		namespace impl
		{
			struct SteamUdpSocketImpl
			{
			};
		}
	};

	namespace fox
	{
		struct SharedString
		{
			char* buffer;
		};
	}

	namespace tpp::net
	{
		struct CmdGetFobTargetDetailResult
		{
			char __pad0[7144];
			std::uint64_t steam_id;
			char __pad1[8];
			fox::SharedString* ip;
			std::uint32_t port;
		};
	}

	static_assert(offsetof(tpp::net::CmdGetFobTargetDetailResult, steam_id) == 7144);
	static_assert(offsetof(tpp::net::CmdGetFobTargetDetailResult, ip) == 7160);
	static_assert(offsetof(tpp::net::CmdGetFobTargetDetailResult, port) == 7168);

	namespace lua
	{
		struct lua_state_container
		{
			void* unk;
			lua_State* state;
			// ...
		};
	}
}
