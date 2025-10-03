#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	// Functions

	namespace fox
	{
		WEAK symbol<fox::impl::TimeSystemImpl()> GetTimeSystem{0x1400F28F0, 0x1400F3940};
		
		WEAK symbol<bool(fox::RawKeyboardData*, RAWKEYBOARD*)> GetRawKeyboardData{0x141A45940, 0x1414A1D60};

		namespace KeyboardListener_
		{
			WEAK symbol<void(fox::RawKeyboardData*)> SetRawKeyData{0x140030BB0, 0x1400304F0};
		}

		namespace Buffer_
		{
			WEAK symbol<size_t(fox::Buffer* buffer)> GetSize{0x140A839C0, 0x1406C4D70};
			WEAK symbol<char* (fox::Buffer* buffer)> GetBuffer{0x14C240CB0, 0x14CB3BB00};
			WEAK symbol<void (fox::Buffer* buffer)> Clear{0x141A43FA0, 0x0};
			WEAK symbol<size_t(fox::Buffer* buffer, const void*, size_t)> Write{0x141A44180, 0x0};
			WEAK symbol<void (fox::Buffer* buffer, size_t)> Resize{0x141A44050, 0x0};
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
				WEAK symbol<UixUtilityImpl* ()> GetUixUtilityToFeedQuarkEnvironment{0x14050B2A0, 0x1402BADD0};
			}
		}

		namespace ui
		{
			namespace ModelNode_
			{
				WEAK symbol<bool(fox::ui::ModelNode*)> GetWorldVisible{0x141DACF90, 0x0};
			}

			namespace ModelNodeText_
			{
				WEAK symbol<void(fox::ui::ModelNodeText*)> ReleaseBufferAndCache{0x141DB5F70, 0x0};
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
					WEAK symbol<char(void* a1, const char* text, char a3, char a4, char a5)> CallLogView{0x14093E510, 0x1463CA640};
					WEAK symbol<char(AnnounceLogViewer*, char)> ModelInit{0x14093E810, 0x1406C9150};
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

					namespace MotherBaseDeviceSystemImpl_
					{
						WEAK symbol<bool()> IsDeviceOpend{0x145DD1FA0, 0x148E4AB20};
					}
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

				WEAK symbol<bool(fox::ui::ModelNodeText*, fox::ui::TextUnit*, const char*)> SetTextForModelNodeText{0x1409171D0, 0x140689CD0};

				namespace UiUtilityImpl_
				{
					WEAK symbol<void()> CallFoxQuit{0x145FD5EA0, 0x146204DC0};
				}
			}
		}

		namespace net
		{
			namespace ServerManager_
			{
				WEAK symbol<FobTarget*(ServerManager*)> GetFobTarget{0x1407D57C0, 0x0};
				WEAK symbol<ServerManager*> s_instance{0x142BEFBF0, 0x0};
			}

			namespace FobTarget_
			{
				WEAK symbol<char(FobTarget*)> CreateHostSession{0x1459F3E20, 0x0};
				WEAK symbol<char(FobTarget*, SessionConnectInfo*)> CreateClientSession{0x1459F3910, 0x0};
			}
		}
	}

	namespace lua
	{
		WEAK symbol<int(lua_State* s, const char* buffer, 
			size_t size, const char* name)> luaL_loadbuffer{0x14C200F90, 0x1414750E0};

		WEAK symbol<int(lua_State* s, const char*)> luaL_loadstring{0x14C201320, 0x14CA4F750};
		WEAK symbol<int(lua_State* s, int, int)> lua_call{0x141A10F30, 0x14146D200};
		WEAK symbol<int(lua_State* s, int, int, int)> lua_pcall{0x141A11930, 0x14CA24AB0};

		WEAK symbol<void(lua_State* L, int, const char*)> lua_getfield{0x141A11450, 0x14CA200E0};
		WEAK symbol<void(lua_State* L, int, const char*)> lua_setfield{0x141A12070, 0x14CA2A3B0};

		WEAK symbol<int(lua_State* L, int)> lua_type{0x141A12570, 0x14146E850};
		WEAK symbol<const char*(lua_State* L, int)> lua_typename{0x141A125A0, 0x14146E880};
		WEAK symbol<int(lua_State* L, int)> lua_next{0x141A11870, 0x14146DB40};
		WEAK symbol<void(lua_State* L, int)> lua_settop{0x141A121E0, 0x14146E4B0};
		WEAK symbol<void(lua_State* L, int)> lua_remove{0x141A11E90, 0x14146E160};

		WEAK symbol<bool(lua_State* L, int)> lua_toboolean{0x141A12330, 0x14CA2BC80};
		WEAK symbol<int(lua_State* L, int)> lua_tointeger{0x141A12390, 0x14146E670};
		WEAK symbol<const char* (lua_State* L, int idx, size_t* len)> lua_tolstring{0x14C1ECA70, 0x14CA2C890};

		WEAK symbol<void(lua_State* L)> lua_pushnil{0x141A11BA0, 0x14146DE70};
		WEAK symbol<void(lua_State* L, const char*)> lua_pushstring{0x141A11BE0, 0x14146DEB0};
		WEAK symbol<void(lua_State* L, double)> lua_pushnumber{0x141A11BC0, 0x14146DE90};
		WEAK symbol<void(lua_State* L, int)> lua_pushinteger{0x141A11BC0, 0x14146DDC0};
		WEAK symbol<void(lua_State* L, int)> lua_pushboolean{0x141A11BC0, 0x14146DC90};

#undef lua_pop
#define lua_pop(L,n) game::lua::lua_settop(L, -(n)-1)
	}

	// Variables

	WEAK symbol<ISteamNetworking*(*)()> SteamNetworking{0x14DB4F7C8, 0x14CE237E8};
	WEAK symbol<ISteamFriends*(*)()> SteamFriends{0x14DB4F7C0, 0x14CE237D8};
	WEAK symbol<ISteamMatchmaking*(*)()> SteamMatchmaking{0x0, 0x14CE237E0};
	WEAK symbol<ISteamUser*(*)()> SteamUser{0x14DB4F7D8, 0x14CE237F8};

	WEAK symbol<fox::nt::impl::SessionImpl2*> s_pSession{0x142B9DFB0, 0x14209B430};
	WEAK symbol<fox::nt::impl::SessionIdle*> s_idleSession{0x142B9E000, 0x14209B480};

	WEAK symbol<mgo_match_container_t*> s_unk1{0x0, 0x141F8A930};
	WEAK symbol<mgo_matchmaking_manager> s_MgoMatchmakingManager{0x0, 0x142054190};

	WEAK symbol<ID3D11Device*> s_deviceD3D{0x142C6B860, 0x1420E4210};
	WEAK symbol<ID3D11DeviceContext*> s_immediateContextD3D{0x142C6B868, 0x1420E4218};

	WEAK symbol<bool> g_needShowCursor{0x0, 0x141F69E66};

	WEAK symbol<fox::Lua*> s_instances{0x142B641F0, 0x141F28240};

	namespace fox
	{
		WEAK symbol<int> g_vkKeyTable{0x142C6C2C0, 0x1420E4C70};
	}
}
