#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	// Functions
	
	namespace fox::Buffer
	{
		WEAK symbol<size_t(void* buffer)> GetSize{0x140A839C0, 0x1406C4D70};
		WEAK symbol<char*(void* buffer)> GetBuffer{0x14C240CB0, 0x14CB3BB00};
	}

	namespace tpp::ui::hud::CommonDataManager
	{
		struct CommonDataManager;

		WEAK symbol<CommonDataManager*()> GetInstance{0x145C0A890, 0x0};
		WEAK symbol<void(CommonDataManager*, const char*, int, int, int)> AnnounceLogView{0x140863050, 0x0};
	}

	namespace fox::nio
	{
		WEAK symbol<IpPortAddress*(const Address*)> DynamicCast_IpPortAddress{0x141A51A30, 0x0};
		WEAK symbol<sockaddr*(IpPortAddress*)> IpPortAddress_GetSockAddr{0x1418CD24D, 0x0};
		WEAK symbol<void(IpPortAddress*)> IpPortAddress_constructor{0x141A518A0, 0x0};
		WEAK symbol<void(IpPortAddress*, const sockaddr*)> IpPortAddress_assign_sockaddr{0x141A51900, 0x0};

		namespace impl
		{
			WEAK symbol<void(SteamUdpSocketImpl* this_, void* request)> SteamUdpSocketImpl_OnP2pSessionRequest{0x141A5FD50, 0x0};
			WEAK symbol<void(SteamUdpSocketImpl* this_, void* request)> SteamUdpSocketImpl_OnP2pSessionConnectFail{0x141A5FCA0, 0x0};
		}
	}

	// Variables

	WEAK symbol<ISteamNetworking*(*)()> SteamNetworking{0x14DB4F7C8, 0x14CE237E8};

	namespace lua
	{
		WEAK symbol<int(lua_State* s, const char* buffer, 
			size_t size, const char* name)> luaL_loadbuffer{0x14C200F90, 0x0};

		WEAK symbol<const char*(lua_State* L, int idx, size_t* len)> lua_tolstring{0x14C1ECA70, 0x0};

		WEAK symbol<lua_state_container*> state_container{0x142B641F0, 0x0};
	}
}
