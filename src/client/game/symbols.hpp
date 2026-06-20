#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	// Functions

	namespace fox
	{
		WEAK symbol<fox::impl::TimeSystemImpl()> GetTimeSystem{0x1400F21F0, 0x1400F3E10, 0x1400F2A50, 0x1400F37A0};
		
		WEAK symbol<bool(fox::RawKeyboardData*, RAWKEYBOARD*)> GetRawKeyboardData{0x141A45650, 0x1414A1640, 0x141A45A30, 0x1414A1C60};

		WEAK symbol<void(void*, unsigned int)> FreeAnnotated{0x140015F90, 0x1400160F0, 0x140015EE0, 0x140016020};

		WEAK symbol<QuarkSystemTable*()> GetQuarkSystemTable{0x140BFF050, 0x1408F4910, 0x148B24E40, 0x148051AD0};

		WEAK symbol<game::fox::StringId(const char*, size_t)> FoxStrHash32{0x1400234E0, 0x140022F50, 0x140023220, 0x140022FD0};

		namespace AssetConfiguration_
		{
			WEAK symbol<SharedString*()> GetLanguage{0x140170B50, 0x1401733A0, 0x1401711F0, 0x140172770};
		}

		namespace Color_
		{
			WEAK symbol<unsigned int(Color*)> EncodeUInt32RGBA{0x1401C03D0, 0x140281E40, 0x1401C03E0, 0x140281560};
		}

		namespace KeyboardListener_
		{
			WEAK symbol<void(fox::RawKeyboardData*)> SetRawKeyData{0x140433E50, 0x142643FB0, 0x140030BA0, 0x140030570};
		}

		namespace MouseListener_
		{
			WEAK symbol<void(fox::RawMouseData*)> SetRawData{0x140031AF0, 0x1402826B0, 0x140031C40, 0x140031700};
		}

		namespace Buffer_
		{
			WEAK symbol<size_t(fox::Buffer* buffer)> GetSize{0x141C6C960, 0x140529B10, 0x140EFE0E0, 0x140F54B80};
			WEAK symbol<char*(fox::Buffer* buffer)> GetBuffer{0x141A43CD0, 0x14149FC10, 0x14CACA8A0, 0x14CA27EE0};
		}

		namespace LuaAutoPtr_
		{
			WEAK symbol<void(LuaAutoPtr*, char, char, const char*)> LuaAutoPtr_{0x14006A590, 0x14006AE40, 0x14006A4E0, 0x14006B1C0};
			WEAK symbol<void(LuaAutoPtr*)> LuaAutoPtr__destructor{0x14006A960, 0x14006B210, 0x14006A8B0, 0x14006B590};
		}

		namespace fs
		{
			namespace PathCodeImpl_
			{
				WEAK symbol<__int64(const char*)> FromString{0x14003DB30, 0x14003D560, 0x14003DE50, 0x14003DC30};
			}
		}

		namespace nt
		{
			namespace Session
			{
				WEAK symbol<::game::Session*()> GetMainSession{0x140313780, 0x140C4E630, 0x140313C50, 0x149530140};
			}

			namespace Member_
			{
				WEAK symbol<void(Member*)> Reset{0x1403174F0, 0x140C541E0, 0x1403179A0, 0x140C546A0};
			}
		}

		namespace uix
		{
			namespace impl
			{
				WEAK symbol<UixUtilityImpl*()> GetUixUtilityToFeedQuarkEnvironment{0x14050B580, 0x1402BB2A0, 0x14050AE60, 0x1402BADA0};
			}
		}

		namespace ui
		{
			namespace RawDaemon_
			{
				WEAK symbol<FontManager*> s_fontManager{0x142C8FAB8, 0x1420A5658, 0x142C8FA98, 0x1420A56C8};
			}

			namespace Font_
			{
				WEAK symbol<bool(Font*, const char*, float, float, float, 
					Vectormath::Aos::Vector2*, Vectormath::Aos::Vector2*, float*, float*)> CreateText{0x141DF62E0, 0x140E67710, 0x141DF6150, 0x140E67A80};
			}

			namespace FontManager_
			{
				WEAK symbol<FontManager::FontGroup*(FontManager*, StringId, unsigned int)> GetFontGroup{0x141DD5750, 0x140E456B0, 0x141DD5410, 0x140E45B10};
			}
		}

		namespace gr
		{
			namespace dg
			{
				namespace CommandBuffer_
				{
					WEAK symbol<void(CommandBuffer*, int, unsigned int)> SetTexture{0x1402D8AA0, 0x140BC9F50, 0x1402D8E70, 0x140BCA120};
					WEAK symbol<void(CommandBuffer*, int, void*, int)> SetVector{0x1402D8B70, 0x140BCA020, 0x1402D8F40, 0x140BCA1F0};
				}

				namespace DynamicVertexBuffer_
				{
					WEAK symbol<char*(DynamicVertexBuffer*, gn::Buffer**, unsigned int*, int)> GetBuffer{0x1402B97E0, 0x140BB9690, 0x1402B9DC0, 0x140BB9890};
				}

				namespace FontSystem_
				{
					WEAK symbol<FontSystem*> m_instance{0x142B93BB8, 0x14208EFC8, 0x142B93BB8, 0x14208F028};

					WEAK symbol<void*(FontSystem*, const char*, int)> RegisterString{0x1402250F0, 0x140B2AEC0, 0x1402256B0, 0x140B2ABB0};
					WEAK symbol<void*(FontSystem*, const char*, int)> UnRegisterString{0x1402261E0, 0x140B2BE70, 0x1402267A0, 0x140B2BB60};
					WEAK symbol<void(FontSystem*, float*, float*)> GetHalfPixelWH{0x140224590, 0x140B2A370, 0x140224B60, 0x140B2A060};
					WEAK symbol<unsigned int(FontSystem*)> GetFontTextureHandle{0x140224430, 0x140B2A210, 0x140224A00, 0x140B29F00};
					WEAK symbol<_TextureGlyphData*(FontSystem*)> GetFontTextureGlyphDataTop{0x140224380, 0x140B2A160, 0x140224950, 0x140B29E50};
					WEAK symbol<void(FontTextureMetrics*, _TextureGlyphData*, float, float, float)> CalculateMetrics{0x140223C30, 0x140B29A10, 0x140224140, 0x140B29700};
				}

				namespace plugins
				{
					namespace Draw2DRenderer_
					{
						WEAK symbol<void(Draw2DRenderer*)> InitRenderingState{0x1402E7A80, 0x140BDA320, 0x1402E7F90, 0x140BDA440};
						WEAK symbol<void(Draw2DRenderer*)> InitPerObject{0x1402E7940, 0x140BDA1E0, 0x1402E7E50, 0x140BDA300};

						WEAK symbol<void(Draw2DRenderer*)> UpdateViewMapping{0x1402E95A0, 0x140BDBE10, 0x1402E9AB0, 0x140BDBF30};

						WEAK symbol<void(Draw2DRenderer*, int, int, int)> DrawVertices{0x1402E7160, 0x140BD9A00, 0x1402E7670, 0x140BD9B20};

						/* 01 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DLine*)> Execute_Packet2DLine{0x1402E2370, 0x140BD4F40, 0x1402E27E0, 0x140BD5060};
						/* 02 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DTriangle*)> Execute_Packet2DTriangle{0x1402E44F0, 0x140BD70C0, 0x1402E4960, 0x140BD71E0};
						/* 03 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DQuad*)> Execute_Packet2DQuad{0x1402E2D90, 0x140BD5960, 0x1402E3200, 0x140BD5A80};
						/* 04 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DBox*)> Execute_Packet2DBox{0x1402E0070, 0x140BD2C40, 0x1402E04E0, 0x140BD2D60};
						/* 05 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DSprite*)> Execute_Packet2DSprite{0x1402E3D10, 0x140BD68E0, 0x1402E4180, 0x140BD6A00};
						/* 06 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DSprite2D*)> Execute_Packet2DSprite2D{0x1402E38F0, 0x140BD64C0, 0x1402E3D60, 0x140BD65E0};
						/* 07 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DRSprite*)> Execute_Packet2DRSprite{0x1402E30D0, 0x140BD5CA0, 0x1402E3540, 0x140BD5DC0};
						/* 08 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DString*)> ExecuteOnly_Packet2DString{0x1402E4E50, 0x140BD7A20, 0x1402E52C0, 0x140BD7B40};
						/* 09 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DString2D*)> Execute_Packet2DString2D{0x1402E4250, 0x140BD6E20, 0x1402E46C0, 0x140BD6F40};
						/* 10 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DCube*)> Execute_Packet2DCube{0x1402E0640, 0x140BD3210, 0x1402E0AB0, 0x140BD3330};
						/* 11 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DLineStrip*)> Execute_Packet2DLineStrip{0x1402E2560, 0x140BD5130, 0x1402E29D0, 0x140BD5250};
						/* 12 */ template<size_t Count> symbol<void*(Draw2DRenderer*, Packet2DTriangleStrip<Count>*)> Execute_Packet2DTriangleStrip{0x1402E4780, 0x140BD7350, 0x1402E4BF0, 0x140BD7470};
						/* 13 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DViewport*)> Execute_Packet2DViewport{0x1402E4AD0, 0x140BD76A0, 0x1402E4F40, 0x140BD77C0};
						/* 14 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DViewmap*)> Execute_Packet2DViewmap{0x1402E4A70, 0x140BD7640, 0x1402E4EE0, 0x140BD7760};
						/* 15 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DWorldCoords*)> Execute_Packet2DWorldCoords{0x1402E4C70, 0x140BD7840, 0x1402E50E0, 0x140BD7960};
						/* 16 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DScreenCoords*)> Execute_Packet2DScreenCoords{0x1402E37D0, 0x140BD63A0, 0x1402E3C40, 0x140BD64C0};
						/* 17 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DMatrix*)> Execute_Packet2DMatrix{0x1402E2830, 0x140BD5400, 0x1402E2CA0, 0x140BD5520};
						/* 18 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DRotation*)> Execute_Packet2DRotation{0x1402E36D0, 0x140BD62A0, 0x1402E3B40, 0x140BD63C0};
						/* 19 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DTranslation*)> Execute_Packet2DTranslation{0x1402E4470, 0x140BD7040, 0x1402E48E0, 0x140BD7160};
						/* 20 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DScale*)> Execute_Packet2DScale{0x1402E3750, 0x140BD6320, 0x1402E3BC0, 0x140BD6440};
						/* 21 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DBillboard*)> Execute_Packet2DBillboard{0x1402DF4B0, 0x140BD2080, 0x1402DF920, 0x140BD21A0};
						/* 22 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DPerspective*)> Execute_Packet2DPerspective{0x1402E28E0, 0x140BD54B0, 0x1402E2D50, 0x140BD55D0};
						/* 23 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DFlat*)> Execute_Packet2DFlat{0x1402E2320, 0x140BD4EF0, 0x1402E2790, 0x140BD5010};
						/* 24 */ WEAK symbol<void*(Draw2DRenderer*, char)> SetDepthMode{0x1402E8230, 0x140BDAAA0, 0x1402E8740, 0x140BDABC0};
						/* 25 */ WEAK symbol<void*(Draw2DRenderer*, char)> SetCullMode{0x1402E8130, 0x140BDA9A0, 0x1402E8640, 0x140BDAAC0};
						/* 26 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DTexture*)> Execute_Packet2DTexture{0x1402E43C0, 0x140BD6F90, 0x1402E4830, 0x140BD70B0};
						/* 27 */ WEAK symbol<void*(Draw2DRenderer*, char)> SetAlpha{0x1402E7E10, 0x140BDA680, 0x1402E8320, 0x140BDA7A0};
						/* 28 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DColor*)> Execute_Packet2DColor{0x1402E04B0, 0x140BD3080, 0x1402E0920, 0x140BD31A0};
						/* 29 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DStencil*)> SetStencil{0x1402E82E0, 0x140BDAB50, 0x1402E87F0, 0x140BDAC70};
						/* 30 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DClearStencil*)> Execute_Packet2DClearStencil{0x1402E0460, 0x140BD3030, 0x1402E08D0, 0x140BD3150};
						/* 32 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DMaterial*)> Execute_Packet2DMaterial{0x1402E2710, 0x140BD52E0, 0x1402E2B80, 0x140BD5400};
						/* 33 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DMaterialWork*)> Execute_Packet2DMaterialWork{0x1402E27E0, 0x140BD53B0, 0x1402E2C50, 0x140BD54D0};
						/* 34 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DUserMatrix*)> Execute_Packet2DUserMatrix{0x1402E48F0, 0x140BD74C0, 0x1402E4D60, 0x140BD75E0};
						/* 38 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DPush*)> Execute_Packet2DPush{0x1402E2D00, 0x140BD58D0, 0x1402E3170, 0x140BD59F0};
						/* 39 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DPop*)> Execute_Packet2DPop{0x1402E2C30, 0x140BD5800, 0x1402E30A0, 0x140BD5920};
						/* 40 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DResolve*)> Execute_Packet2DResolve{0x1402E3670, 0x140BD6240, 0x1402E3AE0, 0x140BD6360};
						/* 41 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DCopyRenderTarget*)> Execute_Packet2DCopyRenderTarget{0x1402E05B0, 0x140BD3180, 0x1402E0A20, 0x140BD32A0};
						/* 45 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DDrawIndices*)> Execute_Packet2DDrawIndices{0x1402E1F80, 0x140BD4B50, 0x1402E23F0, 0x140BD4C70};
						/* 46 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DUserVertexBuffer*)> Execute_Packet2DUserVertexBuffer{0x1402E4960, 0x140BD7530, 0x143E14100, 0x140BD7650};
						/* 47 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DBeginRenderToTemporary*)> Execute_Packet2DBeginRenderToTemporary{0x1402DF350, 0x140BD1F20, 0x1402DF7C0, 0x140BD2040};
						/* 48 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DEndRenderToTemporary*)> Execute_Packet2DEndRenderToTemporary{0x1402E2110, 0x140BD4CE0, 0x1402E2580, 0x140BD4E00};
						/* 49 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DSetTemporaryTexture*)> Execute_Packet2DSetTemporaryTexture{0x1402E3850, 0x140BD6420, 0x1402E3CC0, 0x140BD6540};
					}
				}

				namespace TextureManager_
				{
					WEAK symbol<ResourceId(Path*)> CreateResourceFromFile{0x1402036A0, 0x140AD3750, 0x140203CF0, 0x148C4CF90};
				}

				namespace DgTextureStreamer_
				{
					WEAK symbol<DgTextureStreamer*()> Instance{0x14021D890, 0x140B23700, 0x14021DCC0, 0x140B233F0};
					WEAK symbol<bool(DgTextureStreamer*)> IsEnable{0x14021D980, 0x140B237F0, 0x14021DDB0, 0x140B234E0};
					WEAK symbol<bool(DgTextureStreamer*, ResourceId, char)> RequestTextureDetailByDgTexture{0x14021E720, 0x140B24540, 0x14021EB50, 0x140B24230};
					WEAK symbol<bool(DgTextureStreamer*, ResourceId, char)> UnRequestTextureDetailByDgTexture{0x14021F940, 0x140B25720, 0x14021FD70, 0x140B25410};
				}

				namespace ResourceManagerBase_
				{
					namespace TextureResource_
					{
						WEAK symbol<void(ResourceId)> DeleteResource{0x1401B73F0, 0x1402F67F0, 0x1401B7370, 0x1402F62E0};
					}
				}
			}

			namespace Material_
			{
				WEAK symbol<Material**(Material**, StringId*)> Create{0x140276FF0, 0x140AB3050, 0x140276DB0, 0x140AB3340};
				WEAK symbol<void(Material*, StringId*)> BindShaderTechnique{0x140276E00, 0x140AB2E60, 0x140276BC0, 0x140AB3150};
				WEAK symbol<void(Material*, unsigned int, Texture*)> BindTexture{0x140276EE0, 0x140AB2F40, 0x140276CA0, 0x140AB3230};
				WEAK symbol<void(Material*, unsigned int, Vectormath::Aos::Vector4*)> BindParameter{0x140276B70, 0x140AB2BD0, 0x143BABB60, 0x148B53580};
				WEAK symbol<void(Material*, StringId*)> Material_{0x140276A70, 0x140AB2940, 0x143BAB490, 0x148B52550};
			}

			namespace Texture_
			{
				WEAK symbol<Texture**(Texture**, StringId*)> Create{0x1401B6F50, 0x140AA75E0, 0x1401B6ED0, 0x140AA7DE0};
			}

			namespace Font_
			{
				WEAK symbol<int(const char*, float*, bool, unsigned int)> GetStringWidth{0x141C32A20, 0x140AFF660, 0x141C31E70, 0x140AFF4E0};
			}

			WEAK symbol<void*(dg::StringFontMetricsCache*, const char*, int, int)> InitMetrics{0x1402AF1E0, 0x140BA5990, 0x1402AF6A0, 0x140BA5B60};

			WEAK symbol<void(_fp16*, Vectormath::Aos::Vector4*)> ConvertFloat4toHalf4{0x140302B40, 0x140BF5380, 0x1403030A0, 0x140BF5540};
			WEAK symbol<void(Vectormath::Aos::Vector4*, _fp16*)> ConvertHalf4toFloat4{0x140302D40, 0x140BF5580, 0x141C4EDC0, 0x140C18D00};

			WEAK symbol<unsigned char> g_packetSize{0x1429E5130, 0x141E96100, 0x1429E5130, 0x141E96100};
		}
	}

	namespace tpp
	{
		namespace sys
		{
			namespace WeatherManager_
			{
				WEAK symbol<WeatherManager*> m_instance{0x0, 0x141F69870, 0x0, 0x0};
			}
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
						WEAK symbol<bool()> IsLoading{0x1409D6590, 0x1407B7E50, 0x147F69AF0, 0x1407B7C20};
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
					WEAK symbol<CommonDataManager*()> GetInstance{0x140866910, 0x1405EA330, 0x140865990, 0x1405EA0E0};
					WEAK symbol<void(CommonDataManager*, const char*, int, int, int)> AnnounceLogView{0x140863C60, 0x1405E7270, 0x140862CE0, 0x1405E7020};
					WEAK symbol<bool(CommonDataManager*)> IsEndLoadingTips{0x1408670B0, 0x1405EABC0, 0x140866110, 0x146850D20};
				}

				namespace AnnounceLogViewer_
				{
					WEAK symbol<char(AnnounceLogViewer*, char)> ModelInit{0x14093F280, 0x1406C9080, 0x14093E200, 0x1406C8DB0};
				}
			}

			namespace menu
			{
				namespace UiCommonDataManager_
				{
					WEAK symbol<UiCommonDataManager*()> GetInstance{0x14085FD00, 0x1405E0D00, 0x14085ED60, 0x1405E0950};
					WEAK symbol<int(UiCommonDataManager*)> GetPauseMenuType{0x14085FDC0, 0x1405E0DA0, 0x14085EE40, 0x1405E0A00};
					WEAK symbol<fox::ui::SoundControl*(UiCommonDataManager*)> GetSoundControl{0x1403139E0, 0x1405E0E60, 0x140313E90, 0x1405E0AC0};
				}

				namespace impl
				{
					namespace MotherBaseDeviceSystemImpl_
					{
						WEAK symbol<bool()> IsDeviceOpend{0x14094A1A0, 0x1405E4310, 0x14790EEC0, 0x147EFAF30};
					}
				}
			}

			namespace utility
			{
				WEAK symbol<fox::StringId*(fox::StringId*, unsigned __int16, unsigned __int16, char)> GetMissionNameKey{0x1409131C0, 0x140685520, 0x1409122B0, 0x146AEEC70};
				WEAK symbol<unsigned __int16()> GetCurrentLocationId{0x1409111D0, 0x140683270, 0x1409102C0, 0x140683040};
				WEAK symbol<unsigned __int16()> GetCurrentMissionId{0x140911210, 0x1406832B0, 0x140910300, 0x140683080};
				WEAK symbol<const char*(fox::StringId)> GetLangText{0x140912C10, 0x140684F70, 0x140911D00, 0x140684D40};
				WEAK symbol<fox::StringId*(fox::StringId*, const char*)> GetStringId{0x1409140E0, 0x140686420, 0x1409131D0, 0x1406861F0};

				WEAK symbol<bool(fox::ui::SoundControl*, unsigned int)> StartSound{0x14091B8E0, 0x14068D090, 0x147A8E180, 0x14068CE60};
				WEAK symbol<bool(fox::ui::SoundControl*, unsigned int)> StopSound{0x14091BA10, 0x14068D1C0, 0x147A8E650, 0x14068CF90};

				namespace UiUtilityImpl_
				{
					WEAK symbol<void()> CallFoxQuit{0x1409374C0, 0x1406A3F50, 0x147BE4690, 0x146BD1870};
				}
			}
		}

		namespace net
		{
			namespace ServerManager_
			{
				WEAK symbol<FobTarget*(ServerManager*)> GetFobTarget{0x1407D6470, 0x0, 0x1407D5410, 0x0};
				WEAK symbol<ServerManager*> s_instance{0x142BEFC00, 0x0, 0x142BEFBF0, 0x0};
			}

			namespace FobTarget_
			{
				WEAK symbol<char(FobTarget*)> CreateHostSession{0x1407EE590, 0x0, 0x147441470, 0x0};
				WEAK symbol<char(FobTarget*, SessionConnectInfo*)> CreateClientSession{0x1407EE380, 0x0, 0x147440F30, 0x0};
				WEAK symbol<void(FobTarget*, tpp::mbm::PlayerBasicInfo*, FobTargetInfo*)> ReceiveBasicInfoCommon{0x1407EE910, 0x0, 0x1407EDA00, 0x0};
			}

			namespace Daemon_
			{
				WEAK symbol<Daemon*()> GetInstance{0x1407DDDB0, 0x14057B900, 0x1407DD190, 0x14057B0B0};
			}

			namespace NetworkInfo_
			{
				WEAK symbol<NetworkInfo*> m_instance{0x142BEF840, 0x0, 0x142BEF830, 0x0};
			}

			namespace DisplayName_
			{
				WEAK symbol<void(DisplayName*)> ClearList{0x1407D78C0, 0x0, 0x1407D6890, 0x0};
				WEAK symbol<void(DisplayName*, fox::gk::FirstPartyAccount*)> AddList{0x1407D7890, 0x0, 0x1407D6860, 0x0};
				WEAK symbol<void(DisplayName*)> GetDisplayName{0x1407D7960, 0x0, 0x1407D69E0, 0x0};
			}
		}

		namespace mp
		{
			namespace RulesetManager_
			{
				WEAK symbol<RulesetManager*> s_instance{0x0, 0x14204CAE8, 0x0, 0x14204CB48};
			}
		}
	}

	namespace lua
	{
		WEAK symbol<int(lua_State* s, const char* buffer, 
			size_t size, const char* name)> luaL_loadbuffer{0x141A178E0, 0x141474920, 0x14C9A98C0, 0x141475000};

		WEAK symbol<int(lua_State* s, const char*)> luaL_loadstring{0x141A17B30, 0x141474B70, 0x14C9A9AF0, 0x14C9BE710};
		WEAK symbol<int(lua_State* s, int, int)> lua_call{0x141A10CC0, 0x14146CA40, 0x141A11050, 0x14146D1B0};
		WEAK symbol<int(lua_State* s, int, int, int)> lua_pcall{0x141A116C0, 0x14146D440, 0x141A11A50, 0x14C922AF0};

		WEAK symbol<void(lua_State* L, int, const char*)> lua_getfield{0x140BF2A80, 0x14146CF60, 0x141A11570, 0x14C91E970};
		WEAK symbol<void(lua_State* L, int, const char*)> lua_setfield{0x140BF2F30, 0x14146DB80, 0x141A12190, 0x14C929870};

		WEAK symbol<int(lua_State* L, int)> lua_type{0x141A12300, 0x14146E090, 0x141A12690, 0x14146E800};
		WEAK symbol<const char*(lua_State* L, int)> lua_typename{0x141A12330, 0x14146E0C0, 0x141A126C0, 0x14146E830};
		WEAK symbol<int(lua_State* L, int)> lua_next{0x141A11600, 0x14146D380, 0x141A11990, 0x14146DAF0};
		WEAK symbol<void(lua_State* L, int)> lua_settop{0x141A11F70, 0x14146DCF0, 0x141A12300, 0x14146E460};
		WEAK symbol<void(lua_State* L, int)> lua_remove{0x141A11C20, 0x14146D9A0, 0x141A11FB0, 0x14146E110};

		WEAK symbol<bool(lua_State* L, int)> lua_toboolean{0x141A120C0, 0x14146DE50, 0x141A12450, 0x14C92AEC0};
		WEAK symbol<int(lua_State* L, int)> lua_tointeger{0x141A12120, 0x14146DEB0, 0x141A124B0, 0x14146E620};
		WEAK symbol<double(lua_State* L, int)> lua_tonumber{0x141A121F0, 0x14146DF80, 0x141A12580, 0x14146E6F0};
		WEAK symbol<const char*(lua_State* L, int idx, size_t* len)> lua_tolstring{0x141A12150, 0x14146DEE0, 0x14C992060, 0x14C92BE20};

		WEAK symbol<void(lua_State* L)> lua_pushnil{0x141A11930, 0x14146D6B0, 0x141A11CC0, 0x14146DE20};
		WEAK symbol<void(lua_State* L, const char*)> lua_pushstring{0x141A11970, 0x14146D6F0, 0x14C98DCB0, 0x14146DE60};
		WEAK symbol<void(lua_State* L, double)> lua_pushnumber{0x141A11950, 0x14146D6D0, 0x141A11CE0, 0x14146DE40};
		WEAK symbol<void(lua_State* L, int)> lua_pushinteger{0x141A11880, 0x14146D600, 0x141A11C10, 0x14146DD70};
		WEAK symbol<void(lua_State* L, int)> lua_pushboolean{0x141C9A140, 0x14146D4D0, 0x141A11AE0, 0x14146DC40};

#define game_lua_pop(L,n) game::lua::lua_settop(L, -(n)-1)
	}

	// Variables

	WEAK symbol<ISteamNetworking*(*)()> SteamNetworking{0x14208E138, 0x1416F1100, 0x14E1307C0, 0x14CE0D7E8};
	WEAK symbol<ISteamFriends*(*)()> SteamFriends{0x14208E110, 0x1416F10C0, 0x14E1307B8, 0x14CE0D7D8};
	WEAK symbol<ISteamMatchmaking*(*)()> SteamMatchmaking{0x0, 0x1416F10F8, 0x0, 0x14CE0D7E0};
	WEAK symbol<ISteamUser*(*)()> SteamUser{0x14208E128, 0x1416F1108, 0x14E1307D0, 0x14CE0D7F8};

	WEAK symbol<fox::nt::impl::SessionImpl2*> s_pSession{0x142B9DFB0, 0x14209B420, 0x142B9DFB0, 0x14209B480};
	WEAK symbol<fox::nt::impl::SessionIdle*> s_idleSession{0x142B9E000, 0x14209B470, 0x142B9DFB0, 0x14209B4D0};

	WEAK symbol<mgo_matchmaking_manager> s_mgoMatchMakingManager{0x0, 0x142054160, 0x0, 0x1420541C0};

	WEAK symbol<ID3D11Device*> s_deviceD3D{0x142C6B870, 0x1420E4200, 0x142C6B860, 0x1420E4260};
	WEAK symbol<ID3D11DeviceContext*> s_immediateContextD3D{0x142C6B878, 0x1420E4208, 0x142C6B868, 0x1420E4268};

	WEAK symbol<fox::Lua*> s_instances{0x142B641F0, 0x141F28240, 0x142B641F0, 0x141F28240};

	WEAK symbol<fox::RawKeyboardData> g_rawMouseData{0x142C6C6D8, 0x1420E5068, 0x142C6C6C8, 0x1420E50C8};

	namespace fox
	{
		WEAK symbol<int> g_vkKeyTable{0x142C6C2D0, 0x1420E4C60, 0x142C6C2C0, 0x1420E4CC0};
	}
}
