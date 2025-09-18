#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	// Functions

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

		namespace uix
		{
			namespace impl
			{
				WEAK symbol<UixUtilityImpl* ()> GetUixUtilityToFeedQuarkEnvironment{0x0, 0x1402BADD0};
			}
		}

		namespace EntityHandle_
		{
			WEAK symbol<Entity* (EntityHandle*)> GetRawPtr{0x0, 0x14254B900};
		}
	}

	namespace tpp
	{
		namespace sys
		{
			namespace GameStatus
			{
				WEAK symbol<bool(int)> Check{0x14053DEC0, 0x1402ECAA0};
			}

			namespace GameMode
			{
				WEAK symbol<int()> GetUserMode{0x140546630, 0x1402F5060};
			}
		}

		namespace ui
		{
			namespace hud
			{
				namespace CommonDataManager_
				{
					WEAK symbol<CommonDataManager* ()> GetInstance{0x145C0A890, 0x1405EA6D0};
					WEAK symbol<void(CommonDataManager*, const char*, int, int, int)> AnnounceLogView{0x140863050, 0x1405E7610};
					WEAK symbol<void(CommonDataManager*, const char*, char, char)> AnnounceLogViewOnly{0x140863220, 0x145085A70};
					WEAK symbol<bool(CommonDataManager*)> IsEndLoadingTips{0x140866490, 0x1450A34D0};
				}

				namespace AnnounceLogViewer_
				{
					WEAK symbol<char(void* a1, const char* text, char a3, char a4, char a5)> CallLogView{0x0, 0x1463CA640};
					WEAK symbol<char(AnnounceLogViewer*, char)> ModelInit{0x0, 0x1406C9150};
				}
			}

			namespace menu
			{
				namespace UiCommonDataManager_
				{
					WEAK symbol<UiCommonDataManager* ()> GetInstance{0x14085F0C0, 0x1405E0F30};
					WEAK symbol<int(UiCommonDataManager*)> GetPauseMenuType{0x14085F1C0, 0x1405E0FD0};
					WEAK symbol<fox::ui::SoundControl*(UiCommonDataManager*)> GetSoundControl{0x140314400, 0x1408C2E70};
				}

				namespace impl
				{
					WEAK symbol<MenuSystemImpl> g_menuSystem{0x0, 0x141F9C8F0};
				}
			}

			namespace utility
			{
				WEAK symbol<fox::StringId* (fox::StringId*, unsigned __int16 a2, unsigned __int16 a3, char a4)> GetMissionNameKey{0x140912880, 0x14538F900};
				WEAK symbol<unsigned __int16()> GetCurrentLocationId{0x140910890, 0x140683360};
				WEAK symbol<unsigned __int16()> GetCurrentMissionId{0x1409108D0, 0x1406833A0};
				WEAK symbol<const char* (fox::StringId)> GetLangText{0x1409122D0, 0x140685060};
				WEAK symbol<fox::StringId*(fox::StringId*, const char*)> GetStringId{0x1409137A0, 0x140685060};

				WEAK symbol<bool(fox::ui::SoundControl*, unsigned int)> StartSound{0x14091AFA0, 0x14068D180};
				WEAK symbol<bool(fox::ui::SoundControl*, unsigned int)> StopSound{0x14091B0D0, 0x14068D2B0};

				WEAK symbol<bool(fox::ui::ModelNodeText*, fox::ui::TextUnit*, const char*)> SetTextForModelNodeText{0x0, 0x140689CD0};
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

	WEAK symbol<ISteamNetworking*(*)()> SteamNetworking{0x14DB4F7C8, 0x14CE237E8};
	WEAK symbol<ISteamFriends*(*)()> SteamFriends{0x14DB4F7C0, 0x14CE237D8};
	WEAK symbol<ISteamMatchmaking*(*)()> SteamMatchmaking{0x0, 0x14CE237E0};
	WEAK symbol<ISteamUser*(*)()> SteamUser{0x14DB4F7D8, 0x14CE237F8};

	WEAK symbol<fox::nt::impl::SessionImpl2*> s_pSession{0x142B9DFB0, 0x14209B430};
	WEAK symbol<fox::nt::impl::SessionIdle*> s_idleSession{0x142B9E000, 0x14209B480};

	WEAK symbol<mgo_match_container_t*> s_unk1{0x0, 0x141F8A930};
	WEAK symbol<mgo_matchmaking_t> s_mgoMatchMaking{0x0, 0x142054190};

	WEAK symbol<ID3D11Device*> s_deviceD3D{0x142C6B860, 0x1420E4210};
	WEAK symbol<ID3D11DeviceContext*> s_immediateContextD3D{0x142C6B868, 0x1420E4218};

	WEAK symbol<bool> g_needShowCursor{0x0, 0x141F69E66};

	namespace lua
	{
		WEAK symbol<lua_state_container*> state_container{0x142B641F0, 0x141F28240};
	}
}
