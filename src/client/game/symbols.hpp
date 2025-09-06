#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	// Functions

	namespace tpp::ui::hud::CommonDataManager
	{
		struct CommonDataManager;

		WEAK symbol<CommonDataManager*()> GetInstance{0x145C0A890, 0x0};
		WEAK symbol<void(CommonDataManager*, const char*, int, int, int)> AnnounceLogView{0x140863050, 0x0};
	}

	namespace fox
	{
		namespace Buffer
		{
			WEAK symbol<size_t(void* buffer)> GetSize{0x140A839C0, 0x1406C4D70};
			WEAK symbol<char* (void* buffer)> GetBuffer{0x14C240CB0, 0x14CB3BB00};
		}

		namespace nt
		{
			namespace Session
			{
				WEAK symbol<::game::Session*()> GetMainSession{0x0, 0x14A522BB0};
			}

			namespace Member_
			{
				WEAK symbol<void(Member*)> Reset{0x0, 0x140C54BD0};
			}
		}
	}

	namespace lua
	{
		WEAK symbol<int(lua_State* s, const char* buffer, 
			size_t size, const char* name)> luaL_loadbuffer{0x14C200F90, 0x0};

		WEAK symbol<const char*(lua_State* L, int idx, size_t* len)> lua_tolstring{0x14C1ECA70, 0x0};
	}

	// Variables

	WEAK symbol<ISteamNetworking* (*)()> SteamNetworking{0x14DB4F7C8, 0x14CE237E8};
	WEAK symbol<ISteamFriends* (*)()> SteamFriends{0x14DB4F7C0, 0x14CE237D8};
	WEAK symbol<ISteamMatchmaking* (*)()> SteamMatchmaking{0x0, 0x14CE237E0};

	WEAK symbol<fox::nt::impl::SessionImpl2*> s_pSession{0x0, 0x14209B430};
	WEAK symbol<fox::nt::impl::SessionIdle*> s_idleSession{0x0, 0x14209B480};

	namespace lua
	{
		WEAK symbol<lua_state_container*> state_container{0x142B641F0, 0x0};
	}
}
