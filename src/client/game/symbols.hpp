#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	// Functions

	namespace fox
	{
		WEAK symbol<fox::impl::TimeSystemImpl()> GetTimeSystem{0x1400F28F0, 0x1400F3940, 0x1400F2A50, 0x1400F37A0};
		
		WEAK symbol<bool(fox::RawKeyboardData*, RAWKEYBOARD*)> GetRawKeyboardData{0x141A45940, 0x1414A1D60, 0x141A45A30, 0x1414A1C60};

		WEAK symbol<void(void*, unsigned int)> FreeAnnotated{0x140015EF0, 0x140016020, 0x140015EE0, 0x140016020};

		namespace AssetConfiguration_
		{
			WEAK symbol<SharedString*()> GetLanguage{0x140171180, 0x1401727C0, 0x1401711F0, 0x140172770};
		}

		namespace Color_
		{
			WEAK symbol<unsigned int(Color*)> EncodeUInt32RGBA{0x1401C0350, 0x1402815D0, 0x1401C03E0, 0x140281560};
		}

		namespace KeyboardListener_
		{
			WEAK symbol<void(fox::RawKeyboardData*)> SetRawKeyData{0x140030BB0, 0x1400304F0, 0x140030BA0, 0x140030570};
		}

		namespace MouseListener_
		{
			WEAK symbol<void(fox::RawMouseData*)> SetRawData{0x140031C50, 0x140031670, 0x140031C40, 0x140031700};
		}

		namespace Buffer_
		{
			WEAK symbol<size_t(fox::Buffer* buffer)> GetSize{0x140A839C0, 0x1406C4D70, 0x140EFE0E0, 0x140F54B80};
			WEAK symbol<char* (fox::Buffer* buffer)> GetBuffer{0x14C240CB0, 0x14CB3BB00, 0x14CACA8A0, 0x14CA27EE0};
		}

		namespace nt
		{
			namespace Session
			{
				WEAK symbol<::game::Session*()> GetMainSession{0x140314190, 0x14A522BB0, 0x140313C50, 0x149530140};
			}

			namespace Member_
			{
				WEAK symbol<void(Member*)> Reset{0x140317F20, 0x140C54BD0, 0x1403179A0, 0x140C546A0};
			}
		}

		namespace uix
		{
			namespace impl
			{
				WEAK symbol<UixUtilityImpl* ()> GetUixUtilityToFeedQuarkEnvironment{0x14050B2A0, 0x1402BADD0, 0x14050AE60, 0x1402BADA0};
			}
		}

		namespace ui
		{
			namespace RawDaemon_
			{
				WEAK symbol<FontManager*> s_fontManager{0x142C8FA98, 0x1420A5668, 0x0, 0x0};
			}

			namespace Font_
			{
				WEAK symbol<bool(Font*, const char*, float, float, float, 
					Vectormath::Aos::Vector2*, Vectormath::Aos::Vector2*, float*, float*)> CreateText{0x141DF6110, 0x140E67FD0, 0x0, 0x0};
			}

			namespace FontManager_
			{
				WEAK symbol<FontManager::FontGroup*(FontManager*, StringId, unsigned int)> GetFontGroup{0x141DD53E0, 0x14B35A370, 0x0, 0x0};
			}
		}

		namespace gr
		{
			namespace dg
			{
				namespace CommandBuffer_
				{
					WEAK symbol<void(CommandBuffer*, int, unsigned int)> SetTexture{0x1402D93C0, 0x140BCAAD0, 0x1402D8E70, 0x140BCA120};
					WEAK symbol<void(CommandBuffer*, int, void*, int)> SetVector{0x1402D9490, 0x140BCABA0, 0x1402D8F40, 0x140BCA1F0};
				}

				namespace DynamicVertexBuffer_
				{
					WEAK symbol<char*(DynamicVertexBuffer*, gn::Buffer**, unsigned int*, int)> GetBuffer{0x1402BA320, 0x140BBA240, 0x1402B9DC0, 0x140BB9890};
				}

				namespace FontSystem_
				{
					WEAK symbol<FontSystem*> m_instance{0x142B93BB8, 0x14208EFD8, 0x142B93BB8, 0x14208F028};

					WEAK symbol<void*(FontSystem*, const char*, int)> RegisterString{0x140225760, 0x140B2B600, 0x1402256B0, 0x140B2ABB0};
					WEAK symbol<void*(FontSystem*, const char*, int)> UnRegisterString{0x140226850, 0x140B2C5B0, 0x1402267A0, 0x140B2BB60};
					WEAK symbol<void(FontSystem*, float*, float*)> GetHalfPixelWH{0x140224C00, 0x140B2AAB0, 0x140224B60, 0x140B2A060};
					WEAK symbol<unsigned int(FontSystem*)> GetFontTextureHandle{0x140224AA0, 0x140B2A950, 0x140224A00, 0x140B29F00};
					WEAK symbol<_TextureGlyphData*(FontSystem*)> GetFontTextureGlyphDataTop{0x1402249F0, 0x140B2A8A0, 0x0, 0x0};
					WEAK symbol<void(FontTextureMetrics*, _TextureGlyphData*, float, float, float)> CalculateMetrics{0x1402241E0, 0x140B2A150, 0x140224140, 0x140B29700};
				}

				namespace plugins
				{
					namespace Draw2DRenderer_
					{
						WEAK symbol<void(Draw2DRenderer*)> InitRenderingState{0x1402E84F0, 0x140BDAE40, 0x1402E7F90, 0x140BDA440};
						WEAK symbol<void(Draw2DRenderer*)> InitPerObject{0x1402E83B0, 0x140BDAD00, 0x1402E7E50, 0x140BDA300};

						WEAK symbol<void(Draw2DRenderer*)> UpdateViewMapping{0x1402EA010, 0x140BDC930, 0x1402E9AB0, 0x140BDBF30};

						WEAK symbol<void(Draw2DRenderer*, int, int, int)> DrawVertices{0x1402E7BD0, 0x140BDA520, 0x1402E7670, 0x140BD9B20};

						/* 01 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DLine*)> Execute_Packet2DLine{0x1402E2D40, 0x140BD5A60, 0x1402E27E0, 0x140BD5060};
						/* 02 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DTriangle*)> Execute_Packet2DTriangle{0x1402E4EC0, 0x140BD7BE0, 0x1402E4960, 0x140BD71E0};
						/* 03 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DQuad*)> Execute_Packet2DQuad{0x1402E3760, 0x140BD6480, 0x1402E3200, 0x140BD5A80};
						/* 04 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DBox*)> Execute_Packet2DBox{0x1402E0A40, 0x140BD3760, 0x1402E04E0, 0x140BD2D60};
						/* 05 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DSprite*)> Execute_Packet2DSprite{0x1402E46E0, 0x140BD7400, 0x1402E4180, 0x140BD6A00};
						/* 06 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DSprite2D*)> Execute_Packet2DSprite2D{0x1402E42C0, 0x140BD6FE0, 0x1402E3D60, 0x140BD65E0};
						/* 07 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DRSprite*)> Execute_Packet2DRSprite{0x1402E3AA0, 0x140BD6FE0, 0x1402E3540, 0x140BD5DC0};
						/* 08 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DString*)> ExecuteOnly_Packet2DString{0x1402E5820, 0x140BD8540, 0x1402E52C0, 0x140BD7B40};
						/* 09 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DString2D*)> Execute_Packet2DString2D{0x1402E4C20, 0x140BD7940, 0x1402E46C0, 0x140BD6F40};
						/* 10 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DCube*)> Execute_Packet2DCube{0x1402E1010, 0x140BD3D30, 0x1402E0AB0, 0x140BD3330};
						/* 11 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DLineStrip*)> Execute_Packet2DLineStrip{0x1402E2F30, 0x140BD5C50, 0x1402E29D0, 0x140BD5250};
						/* 12 */ template<size_t Count> symbol<void*(Draw2DRenderer*, Packet2DTriangleStrip<Count>*)> Execute_Packet2DTriangleStrip{0x1402E5150, 0x140BD7E70, 0x1402E4BF0, 0x140BD7470};
						/* 13 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DViewport*)> Execute_Packet2DViewport{0x1402E54A0, 0x140BD81C0, 0x1402E4F40, 0x140BD77C0};
						/* 14 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DViewmap*)> Execute_Packet2DViewmap{0x1402E5440, 0x140BD8160, 0x1402E4EE0, 0x140BD7760};
						/* 15 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DWorldCoords*)> Execute_Packet2DWorldCoords{0x1402E5640, 0x140BD8360, 0x1402E50E0, 0x140BD7960};
						/* 16 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DScreenCoords*)> Execute_Packet2DScreenCoords{0x1402E41A0, 0x140BD6EC0, 0x1402E3C40, 0x140BD64C0};
						/* 17 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DMatrix*)> Execute_Packet2DMatrix{0x1402E3200, 0x140BD5F20, 0x1402E2CA0, 0x140BD5520};
						/* 18 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DRotation*)> Execute_Packet2DRotation{0x1402E40A0, 0x140BD6DC0, 0x1402E3B40, 0x140BD63C0};
						/* 19 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DTranslation*)> Execute_Packet2DTranslation{0x1402E4E40, 0x140BD7B60, 0x1402E48E0, 0x140BD7160};
						/* 20 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DScale*)> Execute_Packet2DScale{0x1402E4120, 0x140BD6E40, 0x1402E3BC0, 0x140BD6440};
						/* 21 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DBillboard*)> Execute_Packet2DBillboard{0x1402DFE80, 0x140BD2BA0, 0x1402DF920, 0x140BD21A0};
						/* 22 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DPerspective*)> Execute_Packet2DPerspective{0x1402E32B0, 0x140BD5FD0, 0x1402E2D50, 0x140BD55D0};
						/* 23 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DFlat*)> Execute_Packet2DFlat{0x1402E2CF0, 0x140BD5A10, 0x1402E2790, 0x140BD5010};
						/* 24 */ WEAK symbol<void*(Draw2DRenderer*, char)> SetDepthMode{0x1402E8CA0, 0x140BDB5C0, 0x1402E8740, 0x140BDABC0};
						/* 25 */ WEAK symbol<void*(Draw2DRenderer*, char)> SetCullMode{0x1402E8BA0, 0x140BDB4C0, 0x1402E8640, 0x140BDAAC0};
						/* 26 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DTexture*)> Execute_Packet2DTexture{0x1402E4D90, 0x140BD7AB0, 0x1402E4830, 0x140BD70B0};
						/* 27 */ WEAK symbol<void*(Draw2DRenderer*, char)> SetAlpha{0x1402E8880, 0x140BDB1A0, 0x1402E8320, 0x140BDA7A0};
						/* 28 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DColor*)> Execute_Packet2DColor{0x1402E0E80, 0x140BD3BA0, 0x1402E0920, 0x140BD31A0};
						/* 29 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DStencil*)> SetStencil{0x1402E8D50, 0x140BDB670, 0x1402E87F0, 0x140BDAC70};
						/* 30 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DClearStencil*)> Execute_Packet2DClearStencil{0x1402E0E30, 0x140BD3B50, 0x1402E08D0, 0x140BD3150};
						/* 32 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DMaterial*)> Execute_Packet2DMaterial{0x1402E30E0, 0x140BD5E00, 0x1402E2B80, 0x140BD5400};
						/* 33 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DMaterialWork*)> Execute_Packet2DMaterialWork{0x1402E31B0, 0x140BD5ED0, 0x1402E2C50, 0x140BD54D0};
						/* 34 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DUserMatrix*)> Execute_Packet2DUserMatrix{0x1402E52C0, 0x140BD7FE0, 0x1402E4D60, 0x140BD75E0};
						/* 38 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DPush*)> Execute_Packet2DPush{0x1402E36D0, 0x140BD63F0, 0x1402E3170, 0x140BD59F0};
						/* 39 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DPop*)> Execute_Packet2DPop{0x1402E3600, 0x140BD6320, 0x1402E30A0, 0x140BD5920};
						/* 40 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DResolve*)> Execute_Packet2DResolve{0x1402E4040, 0x140BD6D60, 0x1402E3AE0, 0x140BD6360};
						/* 41 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DCopyRenderTarget*)> Execute_Packet2DCopyRenderTarget{0x1402E0F80, 0x140BD3CA0, 0x1402E0A20, 0x140BD32A0};
						/* 45 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DDrawIndices*)> Execute_Packet2DDrawIndices{0x1402E2950, 0x140BD5670, 0x1402E23F0, 0x140BD4C70};
						/* 46 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DUserVertexBuffer*)> Execute_Packet2DUserVertexBuffer{0x1402E5330, 0x140BD8050, 0x143E14100, 0x140BD7650};
						/* 47 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DBeginRenderToTemporary*)> Execute_Packet2DBeginRenderToTemporary{0x1402DFD20, 0x140BD2A40, 0x1402DF7C0, 0x140BD2040};
						/* 48 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DEndRenderToTemporary*)> Execute_Packet2DEndRenderToTemporary{0x1402E2AE0, 0x140BD5800, 0x1402E2580, 0x140BD4E00};
						/* 49 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DSetTemporaryTexture*)> Execute_Packet2DSetTemporaryTexture{0x1402E4220, 0x140BD6F40, 0x1402E3CC0, 0x140BD6540};
					}
				}
			}

			namespace Font_
			{
				WEAK symbol<int(const char*, float*, bool, unsigned int)> GetStringWidth{0x141C32000, 0x140B00030, 0x141C31E70, 0x140AFF4E0};
			}

			WEAK symbol<void*(dg::StringFontMetricsCache*, const char*, int, int)> InitMetrics{0x1402AFB70, 0x140BA6560, 0x1402AF6A0, 0x140BA5B60};

			WEAK symbol<void(_fp16*, Vectormath::Aos::Vector4*)> ConvertFloat4toHalf4{0x140303600, 0x140BF5F40, 0x1403030A0, 0x140BF5540};
			WEAK symbol<void(Vectormath::Aos::Vector4*, _fp16*)> ConvertHalf4toFloat4{0x141C4EE50, 0x14050CFA0, 0x141C4EDC0, 0x140C18D00};

			WEAK symbol<unsigned char> g_packetSize{0x1429E5130, 0x141E96100, 0x1429E5130, 0x141E96100};
		}
	}

	namespace tpp
	{
		namespace sys
		{

		}

		namespace gm
		{
			namespace player
			{
				WEAK symbol<Player2System> player2System{0x142A82C28, 0x141E7F578, 0x142A82C28, 0x141E7F578};

				namespace impl
				{
					namespace Player2UtilityImpl_
					{
						WEAK symbol<bool()> IsLoading{0x1462AF2F0, 0x1407B8290, 0x147F69AF0, 0x1407B7C20};
					}
				}
			}
		}

		namespace ui
		{
			namespace hud
			{
				namespace CommonDataManager_
				{
					WEAK symbol<CommonDataManager* ()> GetInstance{0x145C0A890, 0x1405EA6D0, 0x140865990, 0x1405EA0E0};
					WEAK symbol<void(CommonDataManager*, const char*, int, int, int)> AnnounceLogView{0x140863050, 0x1405E7610, 0x140862CE0, 0x1405E7020};
					WEAK symbol<void(CommonDataManager*, const char*, char, char)> AnnounceLogViewOnly{0x140863220, 0x145085A70, 0x140862EB0, 0x146834CD0};
					WEAK symbol<bool(CommonDataManager*)> IsEndLoadingTips{0x140866490, 0x1450A34D0, 0x140866110, 0x146850D20};
				}

				namespace AnnounceLogViewer_
				{
					WEAK symbol<char(void* a1, const char* text, char a3, char a4, char a5)> CallLogView{0x14093E510, 0x1463CA640, 0x147C2A370, 0x146DFB540};
					WEAK symbol<char(AnnounceLogViewer*, char)> ModelInit{0x14093E810, 0x1406C9150, 0x14093E200, 0x1406C8DB0};
				}
			}

			namespace menu
			{
				namespace UiCommonDataManager_
				{
					WEAK symbol<UiCommonDataManager* ()> GetInstance{0x14085F0C0, 0x1405E0F30, 0x14085ED60, 0x1405E0950};
					WEAK symbol<int(UiCommonDataManager*)> GetPauseMenuType{0x14085F1C0, 0x1405E0FD0, 0x14085EE40, 0x1405E0A00};
					WEAK symbol<fox::ui::SoundControl*(UiCommonDataManager*)> GetSoundControl{0x140314400, 0x1408C2E70, 0x140313E90, 0x1405E0AC0};
				}

				namespace impl
				{
					namespace MotherBaseDeviceSystemImpl_
					{
						WEAK symbol<bool()> IsDeviceOpend{0x145DD1FA0, 0x148E4AB20, 0x14790EEC0, 0x147EFAF30};
					}
				}
			}

			namespace utility
			{
				WEAK symbol<fox::StringId* (fox::StringId*, unsigned __int16, unsigned __int16, char)> GetMissionNameKey{0x140912880, 0x14538F900, 0x1409122B0, 0x146AEEC70};
				WEAK symbol<unsigned __int16()> GetCurrentLocationId{0x140910890, 0x140683360, 0x1409102C0, 0x140683040};
				WEAK symbol<unsigned __int16()> GetCurrentMissionId{0x1409108D0, 0x1406833A0, 0x140910300, 0x140683080};
				WEAK symbol<const char* (fox::StringId)> GetLangText{0x1409122D0, 0x140685060, 0x140911D00, 0x140684D40};
				WEAK symbol<fox::StringId*(fox::StringId*, const char*)> GetStringId{0x1409137A0, 0x140686510, 0x1409131D0, 0x1406861F0};

				WEAK symbol<bool(fox::ui::SoundControl*, unsigned int)> StartSound{0x14091AFA0, 0x14068D180, 0x147A8E180, 0x14068CE60};
				WEAK symbol<bool(fox::ui::SoundControl*, unsigned int)> StopSound{0x14091B0D0, 0x14068D2B0, 0x147A8E650, 0x14068CF90};

				WEAK symbol<bool(fox::ui::ModelNodeText*, fox::ui::TextUnit*, const char*)> SetTextForModelNodeText{0x1409171D0, 0x140689CD0, 0x140916C00, 0x1406899B0};

				namespace UiUtilityImpl_
				{
					WEAK symbol<void()> CallFoxQuit{0x145FD5EA0, 0x146204DC0, 0x147BE4690, 0x146BD1870};
				}
			}
		}

		namespace net
		{
			namespace ServerManager_
			{
				WEAK symbol<FobTarget*(ServerManager*)> GetFobTarget{0x1407D57C0, 0x0, 0x0, 0x0};
				WEAK symbol<ServerManager*> s_instance{0x142BEFBF0, 0x0, 0x0, 0x0};
			}

			namespace FobTarget_
			{
				WEAK symbol<char(FobTarget*)> CreateHostSession{0x1459F3E20, 0x0, 0x0, 0x0};
				WEAK symbol<char(FobTarget*, SessionConnectInfo*)> CreateClientSession{0x1459F3910, 0x0, 0x0, 0x0};
			}

			namespace Daemon_
			{
				WEAK symbol<Daemon*()> GetInstance{0x0, 0x14057B6E0, 0x0, 0x14057B0B0};
			}
		}

		namespace mp
		{
			namespace RulsetManager_
			{
				WEAK symbol<RuleSetManager*> s_instance{0x0, 0x14204CB18, 0x0, 0x0};
			}
		}
	}

	namespace lua
	{
		WEAK symbol<int(lua_State* s, const char* buffer, 
			size_t size, const char* name)> luaL_loadbuffer{0x14C200F90, 0x1414750E0, 0x14C9A98C0, 0x141475000};

		WEAK symbol<int(lua_State* s, const char*)> luaL_loadstring{0x14C201320, 0x14CA4F750, 0x14C9A9AF0, 0x14C9BE710};
		WEAK symbol<int(lua_State* s, int, int)> lua_call{0x141A10F30, 0x14146D200, 0x141A11050, 0x14146D1B0};
		WEAK symbol<int(lua_State* s, int, int, int)> lua_pcall{0x141A11930, 0x14CA24AB0, 0x141A11A50, 0x14C922AF0};

		WEAK symbol<void(lua_State* L, int, const char*)> lua_getfield{0x141A11450, 0x14CA200E0, 0x141A11570, 0x14C91E970};
		WEAK symbol<void(lua_State* L, int, const char*)> lua_setfield{0x141A12070, 0x14CA2A3B0, 0x141A12190, 0x14C929870};

		WEAK symbol<int(lua_State* L, int)> lua_type{0x141A12570, 0x14146E850, 0x141A12690, 0x14146E800};
		WEAK symbol<const char*(lua_State* L, int)> lua_typename{0x141A125A0, 0x14146E880, 0x141A126C0, 0x14146E830};
		WEAK symbol<int(lua_State* L, int)> lua_next{0x141A11870, 0x14146DB40, 0x141A11990, 0x14146DAF0};
		WEAK symbol<void(lua_State* L, int)> lua_settop{0x141A121E0, 0x14146E4B0, 0x141A12300, 0x14146E460};
		WEAK symbol<void(lua_State* L, int)> lua_remove{0x141A11E90, 0x14146E160, 0x141A11FB0, 0x14146E110};

		WEAK symbol<bool(lua_State* L, int)> lua_toboolean{0x141A12330, 0x14CA2BC80, 0x141A12450, 0x14C92AEC0};
		WEAK symbol<int(lua_State* L, int)> lua_tointeger{0x141A12390, 0x14146E670, 0x141A124B0, 0x14146E620};
		WEAK symbol<const char* (lua_State* L, int idx, size_t* len)> lua_tolstring{0x14C1ECA70, 0x14CA2C890, 0x14C992060, 0x14C92BE20};

		WEAK symbol<void(lua_State* L)> lua_pushnil{0x141A11BA0, 0x14146DE70, 0x141A11CC0, 0x14146DE20};
		WEAK symbol<void(lua_State* L, const char*)> lua_pushstring{0x141A11BE0, 0x14146DEB0, 0x14C98DCB0, 0x14146DE60};
		WEAK symbol<void(lua_State* L, double)> lua_pushnumber{0x141A11BC0, 0x14146DE90, 0x141A11CE0, 0x14146DE40};
		WEAK symbol<void(lua_State* L, int)> lua_pushinteger{0x141A11AF0, 0x14146DDC0, 0x141A11C10, 0x14146DD70};
		WEAK symbol<void(lua_State* L, int)> lua_pushboolean{0x141A119C0, 0x14146DC90, 0x141A11AE0, 0x14146DC40};

#undef lua_pop
#define lua_pop(L,n) game::lua::lua_settop(L, -(n)-1)
	}

	// Variables

	WEAK symbol<ISteamNetworking*(*)()> SteamNetworking{0x14DB4F7C8, 0x14CE237E8, 0x14E1307C0, 0x14CE0D7E8};
	WEAK symbol<ISteamFriends*(*)()> SteamFriends{0x14DB4F7C0, 0x14CE237D8, 0x14E1307B8, 0x14CE0D7D8};
	WEAK symbol<ISteamMatchmaking*(*)()> SteamMatchmaking{0x0, 0x14CE237E0, 0x0, 0x14CE0D7E0};
	WEAK symbol<ISteamUser*(*)()> SteamUser{0x14DB4F7D8, 0x14CE237F8, 0x14E1307D0, 0x14CE0D7F8};

	WEAK symbol<fox::nt::impl::SessionImpl2*> s_pSession{0x142B9DFB0, 0x14209B430, 0x142B9DFB0, 0x14209B480};
	WEAK symbol<fox::nt::impl::SessionIdle*> s_idleSession{0x142B9E000, 0x14209B480, 0x142B9DFB0, 0x14209B4D0};

	WEAK symbol<mgo_matchmaking_manager> s_mgoMatchMakingManager{0x0, 0x142054190, 0x0, 0x1420541C0};

	WEAK symbol<ID3D11Device*> s_deviceD3D{0x142C6B860, 0x1420E4210, 0x0, 0x0};
	WEAK symbol<ID3D11DeviceContext*> s_immediateContextD3D{0x142C6B868, 0x1420E4218, 0x0, 0x0};

	WEAK symbol<fox::Lua*> s_instances{0x142B641F0, 0x141F28240, 0x142B641F0, 0x141F28240};

	WEAK symbol<fox::RawKeyboardData> g_rawMouseData{0x142C6C6C8, 0x1420E5078, 0x142C6C6C8, 0x1420E50C8};

	namespace fox
	{
		WEAK symbol<int> g_vkKeyTable{0x142C6C2C0, 0x1420E4C70, 0x142C6C2C0, 0x1420E4CC0};
	}
}
