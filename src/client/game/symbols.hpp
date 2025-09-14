#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	// Functions

	namespace tpp::ui::hud::CommonDataManager
	{
		struct CommonDataManager;

		WEAK symbol<CommonDataManager*()> GetInstance{0x145C0A890, 0x1405EA6D0};
		WEAK symbol<void(CommonDataManager*, const char*, int, int, int)> AnnounceLogView{0x140863050, 0x1405E7610};
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
				WEAK symbol<::game::Session*()> GetMainSession{0x140314190, 0x14A522BB0};
			}

			namespace Member_
			{
				WEAK symbol<void(Member*)> Reset{0x140317F20, 0x140C54BD0};
			}
		}
	}

	namespace tpp
	{
		namespace ui
		{
			namespace utility
			{
				WEAK symbol<fox::StringId* (fox::StringId*, unsigned __int16 a2, unsigned __int16 a3, char a4)> GetMissionNameKey{0x140912880, 0x14538F900};
				WEAK symbol<unsigned __int16()> GetCurrentLocationId{0x140910890, 0x140683360};
				WEAK symbol<unsigned __int16()> GetCurrentMissionId{0x1409108D0, 0x1406833A0};
				WEAK symbol<const char* (fox::StringId)> GetLangText{0x1409122D0, 0x140685060};
				WEAK symbol<fox::StringId*(fox::StringId*, const char*)> GetStringId{0x1409137A0, 0x140685060};
			}
		}
	}

	namespace lua
	{
		WEAK symbol<int(lua_State* s, const char* buffer, 
			size_t size, const char* name)> luaL_loadbuffer{0x14C200F90, 0x1414750E0};

		WEAK symbol<const char*(lua_State* L, int idx, size_t* len)> lua_tolstring{0x14C1ECA70, 0x14CA2C890};
	}

	// Variables

	WEAK symbol<ISteamNetworking* (*)()> SteamNetworking{0x14DB4F7C8, 0x14CE237E8};
	WEAK symbol<ISteamFriends* (*)()> SteamFriends{0x14DB4F7C0, 0x14CE237D8};
	WEAK symbol<ISteamMatchmaking* (*)()> SteamMatchmaking{0x0, 0x14CE237E0};

	WEAK symbol<fox::nt::impl::SessionImpl2*> s_pSession{0x142B9DFB0, 0x14209B430};
	WEAK symbol<fox::nt::impl::SessionIdle*> s_idleSession{0x142B9E000, 0x14209B480};

	WEAK symbol<fox::unk1*> s_unk1{0x0, 0x141F8A930};
	WEAK symbol<fox::unk2> s_unk2{0x0, 0x142054190};

	namespace lua
	{
		WEAK symbol<lua_state_container*> state_container{0x142B641F0, 0x141F28240};
	}
}
