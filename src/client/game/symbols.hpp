#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	// Functions

	namespace fox
	{
		WEAK symbol<fox::impl::TimeSystemImpl()> GetTimeSystem{0x1400F21F0, 0x1400F3B50 , 0x0, 0x0};
		
		WEAK symbol<bool(fox::RawKeyboardData*, RAWKEYBOARD*)> GetRawKeyboardData{0x141A45650, 0x1414A16B0, 0x0, 0x0};

		WEAK symbol<void*(unsigned __int64, unsigned int)> KernelAllocAligned{0x140015FC0, 0x1400160A0, 0x0, 0x0};
		WEAK symbol<void(void*, unsigned int)> FreeAnnotated{0x140015F90, 0x1400160F0, 0x0, 0x0};

		WEAK symbol<QuarkSystemTable*()> GetQuarkSystemTable{0x140BFF050, 0x1408F4800, 0x0, 0x0};

		WEAK symbol<game::fox::StringId(const char*, size_t)> FoxStrHash32{0x1400234E0, 0x140022F00, 0x0, 0x0};

		WEAK symbol<unsigned int(unsigned __int8**)> Utf8ToUtf32{0x140023910, 0x140023330, 0x0, 0x0};

		WEAK symbol<__int64(const char*)> GetBinaryStringHandle{0x0, 0x14001D960, 0x0, 0x0};

		WEAK symbol<void(lua::lua_State*, void*)> LuaPushEntity{0x0, 0x1400F91E0, 0x0, 0x0};

		namespace Array_
		{
			WEAK symbol<bool(Array<Path>*, Path*)> Path_PushBack{0x0, 0x1400AC8E0, 0x0, 0x0};
		}

		namespace AssetConfiguration_
		{
			WEAK symbol<SharedString*()> GetLanguage{0x140170B50, 0x140173420, 0x0, 0x0};
		}

		namespace Color_
		{
			WEAK symbol<unsigned int(Color*)> EncodeUInt32RGBA{0x1401C03D0, 0x140281F70, 0x0, 0x0};
		}

		namespace KeyboardListener_
		{
			WEAK symbol<void(fox::RawKeyboardData*)> SetRawKeyData{0x1432C0A60, 0x1426D3540, 0x0, 0x0};
		}

		namespace MouseListener_
		{
			WEAK symbol<void(fox::RawMouseData*)> SetRawData{0x140031AF0, 0x1402827E0, 0x0, 0x0};
		}

		namespace Mutex_
		{
			WEAK symbol<void(Mutex*, int*)> SpinLock{0x141A4CD70, 0x1414A8E50, 0x0, 0x0};
			WEAK symbol<void(Mutex*, int*)> Unlock{0x141A4CD90, 0x140036110, 0x0, 0x0};
		}

		namespace Buffer_
		{
			WEAK symbol<size_t(fox::Buffer* buffer)> GetSize{0x141C6C960, 0x140529B00, 0x0, 0x0};
			WEAK symbol<char*(fox::Buffer* buffer)> GetBuffer{0x141A43CD0, 0x14149FC80, 0x0, 0x0};
		}

		namespace LuaAutoPtr_
		{
			WEAK symbol<void(LuaAutoPtr*, char, char, const char*)> LuaAutoPtr_{0x14006A590, 0x14006AE40, 0x0, 0x0};
			WEAK symbol<void(LuaAutoPtr*)> LuaAutoPtr__destructor{0x14006A960, 0x14006B210, 0x0, 0x0};
		}

		namespace LuaScopedGlobalPolicy_
		{
			WEAK symbol<void(LuaScopedGlobalPolicy*, lua::lua_State*, int)> LuaScopedGlobalPolicy_{0x14006A850, 0x14006B100, 0x0, 0x0};
			WEAK symbol<void(LuaScopedGlobalPolicy*)> LuaScopedGlobalPolicy__destructor{0x14006AA40, 0x14006B2F0, 0x0, 0x0};
		}

		namespace Script_
		{
			WEAK symbol<int*(Script*, int*, __int64*, Lua*, int, int)> CallScriptFunc{0x0, 0x1400EA290, 0x0, 0x0};
		}

		namespace fs
		{
			namespace FileLocationManager_
			{
				WEAK symbol<unsigned int(MountPoint*, unsigned int)> SetIoHandleCount{0x14004A200, 0x14004A2E0, 0x0, 0x0};
			}

			namespace MountPoint_
			{
				WEAK symbol<MountPoint* (const char*, const char*, __int64, int)> CreateWithPackFile{0x140048E20, 0x140049150, 0x0, 0x0};
				WEAK symbol<void(MountPoint*)> Destroy{0x140049270, 0x140049510, 0x0, 0x0};
			}

			namespace PathCodeImpl_
			{
				WEAK symbol<__int64(const char*)> FromString{0x14003DB30, 0x14003D560, 0x0, 0x0};
			}
		}

		namespace gm
		{
			namespace impl
			{
				WEAK symbol<GameObjectMessageSystemImpl*> g_messagesystem{0x142C3A5D0, 0x142057548, 0x0, 0x0};
			}
		}

		namespace nt
		{
			namespace Session_
			{
				WEAK symbol<::game::Session*()> GetMainSession{0x140313780, 0x140C4E650, 0x0, 0x0};
			}

			namespace Member_
			{
				WEAK symbol<void(Member*)> Reset{0x1403174F0, 0x140C54200, 0x0, 0x0};
			}
		}

		namespace ncl
		{
			namespace NclDaemon_
			{
				WEAK symbol<NclDaemon*> s_instance{0x142C87A50, 0x14209B110, 0x0, 0x0};
				WEAK symbol<SharedString* (NclDaemon*, StringId)> GetUrl{0x149240530, 0x140C442E0, 0x0, 0x0};
			}
		}

		namespace uix
		{
			namespace impl
			{
				WEAK symbol<UixUtilityImpl*()> GetUixUtilityToFeedQuarkEnvironment{0x14050B580, 0x1402BB280, 0x0, 0x0};
			}
		}

		namespace ui
		{
			namespace RawDaemon_
			{
				WEAK symbol<FontManager*> s_fontManager{0x142C8FAB8, 0x1420A5658, 0x0, 0x0};
			}

			namespace Font_
			{
				WEAK symbol<bool(Font*, const char*, float, float, float, 
					Vectormath::Aos::Vector2*, Vectormath::Aos::Vector2*, float*, float*)> CreateText{0x141DF62E0, 0x140E67780, 0x0, 0x0};
			}

			namespace FontManager_
			{
				WEAK symbol<FontManager::FontGroup*(FontManager*, StringId, unsigned int)> GetFontGroup{0x141DD5750, 0x140E45720, 0x0, 0x0};
			}
		}

		namespace gr
		{
			WEAK symbol<void*(dg::StringFontMetricsCache*, const char*, int, int)> InitMetrics{0x1402AF1E0, 0x140BA59B0, 0x0, 0x0};

			WEAK symbol<void(_fp16*, Vectormath::Aos::Vector4*)> ConvertFloat4toHalf4{0x140302B40, 0x140BF53A0, 0x0, 0x0};
			WEAK symbol<void(Vectormath::Aos::Vector4*, _fp16*)> ConvertHalf4toFloat4{0x140302D40, 0x140BF55A0, 0x0, 0x0};

			WEAK symbol<unsigned char> g_packetSize{0x1429E5130, 0x141E96100, 0x0, 0x0};

			WEAK symbol<RenderWork*> g_renderWork{0x142B73580, 0x14206C330, 0x0, 0x0};

			namespace Draw2D_
			{
				WEAK symbol<void(Draw2D*)> Draw2D_{0x1401BFCA0, 0x140AAFC80, 0x0, 0x0};
			}

			namespace Material_
			{
				WEAK symbol<Material**(Material**, StringId*)> Create{0x140276FF0, 0x140AB3070, 0x0, 0x0};
				WEAK symbol<void(Material*, StringId*)> BindShaderTechnique{0x140276E00, 0x140AB2E80, 0x0, 0x0};
				WEAK symbol<void(Material*, unsigned int, Texture*)> BindTexture{0x140276EE0, 0x140AB2F60, 0x0, 0x0};
				WEAK symbol<void(Material*, unsigned int, Vectormath::Aos::Vector4*)> BindParameter{0x140276B70, 0x140AB2BF0, 0x0, 0x0};
				WEAK symbol<void(Material*, StringId*)> Material_{0x140276A70, 0x140AB2960, 0x0, 0x0};
			}

			namespace Scene_
			{
				WEAK symbol<Scene*()> GetDefaultScene{0x1401B5920, 0x140AAA730, 0x0, 0x0};
				WEAK symbol<void(Scene*, Draw2D*)> Queue{0x1401B5AE0, 0x140AAA8D0, 0x0, 0x0};
			}

			namespace Texture_
			{
				WEAK symbol<Texture**(Texture**, StringId*)> Create{0x1401B6F50, 0x140AA7600, 0x0, 0x0};
			}

			namespace Font_
			{
				WEAK symbol<int(const char*, float*, bool, unsigned int)> GetStringWidth{0x141C32A20, 0x140AFF660, 0x0, 0x0};
			}

			namespace dg
			{
				WEAK symbol<DgDx11*()> GetDgInstance{0x140204E80, 0x140B145C0, 0x0, 0x0};

				namespace CommandBuffer_
				{
					WEAK symbol<void(CommandBuffer*, int, unsigned int)> SetTexture{0x1402D8AA0, 0x140BC9F70, 0x0, 0x0};
					WEAK symbol<void(CommandBuffer*, int, void*, int)> SetVector{0x1402D8B70, 0x140BCA040, 0x0, 0x0};
				}

				namespace DynamicVertexBuffer_
				{
					WEAK symbol<char*(DynamicVertexBuffer*, gn::Buffer**, unsigned int*, int)> GetBuffer{0x1402B97E0, 0x140BB96B0, 0x0, 0x0};
				}

				namespace ImageGlyphManager_
				{
					WEAK symbol<_TextureGlyphData*(ImageGlyphManager*, unsigned int)> SearchGlyphData{0x140225C20, 0x140B2B8D0, 0x0, 0x0};
					WEAK symbol<_TextureGlyphData*(ImageGlyphManager*, FontData*, _TextureAreaInfo*, unsigned int)> RegisterGlyphData{0x140224F00, 0x140B2ACF0, 0x0, 0x0};
				}

				namespace ImageAreaManager_
				{
					WEAK symbol<_TextureAreaInfo*(ImageAreaManager*, unsigned char, unsigned char, unsigned int)> AllocateAreaInfo{0x1402239C0, 0x140B297C0, 0x0, 0x0};
				}

				namespace FontSystem_
				{
					WEAK symbol<FontSystem*> m_instance{0x142B93BB8, 0x14208EFC8, 0x0, 0x0};

					WEAK symbol<void*(FontSystem*, const char*, int)> RegisterString{0x1402250F0, 0x140B2AEE0, 0x0, 0x0};
					WEAK symbol<void*(FontSystem*, const char*, int)> UnRegisterString{0x1402261E0, 0x140B2BE90, 0x0, 0x0};
					WEAK symbol<void(FontSystem*, float*, float*)> GetHalfPixelWH{0x140224590, 0x140B2A390, 0x0, 0x0};
					WEAK symbol<unsigned int(FontSystem*)> GetFontTextureHandle{0x140224430, 0x140B2A230, 0x0, 0x0};
					WEAK symbol<_TextureGlyphData*(FontSystem*)> GetFontTextureGlyphDataTop{0x140224380, 0x140B2A180, 0x0, 0x0};
					WEAK symbol<void(FontTextureMetrics*, _TextureGlyphData*, float, float, float)> CalculateMetrics{0x140223C30, 0x140B29A30, 0x0, 0x0};
				}

				namespace FontData_
				{
					WEAK symbol<char(FontData*, const char*, const char*)> LoadFontData{0x140224820, 0x140B2A610, 0x0, 0x0};
				}

				namespace FontTextureRender_
				{
					WEAK symbol<void(FontTextureRender*, unsigned int, unsigned int)> FontTextureRender_{0x140222E40, 0x140B28C40, 0x0, 0x0};
					WEAK symbol<void(FontTextureRender*)> UpdateRenderGlyphTexture{0x1402264B0, 0x140B2C160, 0x0, 0x0};
					WEAK symbol<void(FontTextureRender*, FontData*, wchar_t*)> RegisterStringW{0x140225290, 0x140B2B080, 0x0, 0x0};
					WEAK symbol<void(FontTextureRender*, FontData*, FontData_Glyph*, _TextureGlyphData*)> AddTextureRenderOrder{0x1402238D0, 0x140B296D0, 0x0, 0x0};
				}

				namespace TextureManager_
				{
					WEAK symbol<ResourceId(Path*)> CreateResourceFromFile{0x1402036A0, 0x140AD3770, 0x0, 0x0};
				}

				namespace DgTextureStreamer_
				{
					WEAK symbol<DgTextureStreamer* ()> Instance{0x14021D890, 0x140B23720, 0x0, 0x0};
					WEAK symbol<bool(DgTextureStreamer*)> IsEnable{0x14021D980, 0x140B23810, 0x0, 0x0};
					WEAK symbol<bool(DgTextureStreamer*, ResourceId, char)> RequestTextureDetailByDgTexture{0x14021E720, 0x140B24560, 0x0, 0x0};
					WEAK symbol<bool(DgTextureStreamer*, ResourceId, char)> UnRequestTextureDetailByDgTexture{0x14021F940, 0x140B25740, 0x0, 0x0};
				}

				namespace ResourceManagerBase_
				{
					namespace TextureResource_
					{
						WEAK symbol<void(ResourceId)> DeleteResource{0x1401B73F0, 0x1402F67B0, 0x0, 0x0};
					}
				}

				namespace plugins
				{
					namespace Draw2DRenderer_
					{
						WEAK symbol<void(Draw2DRenderer*)> InitRenderingState{0x1402E7A80, 0x140BDA340, 0x0, 0x0};
						WEAK symbol<void(Draw2DRenderer*)> InitPerObject{0x1402E7940, 0x140BDA200, 0x0, 0x0};

						WEAK symbol<void(Draw2DRenderer*)> UpdateViewMapping{0x1402E95A0, 0x140BDBE30, 0x0, 0x0};

						WEAK symbol<void(Draw2DRenderer*, int, int, int)> DrawVertices{0x1402E7160, 0x140BD9A20, 0x0, 0x0};

						/* 01 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DLine*)> Execute_Packet2DLine{0x1402E2370, 0x140BD4F60, 0x0, 0x0};
						/* 02 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DTriangle*)> Execute_Packet2DTriangle{0x1402E44F0, 0x140BD70E0, 0x0, 0x0};
						/* 03 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DQuad*)> Execute_Packet2DQuad{0x1402E2D90, 0x140BD5980, 0x0, 0x0};
						/* 04 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DBox*)> Execute_Packet2DBox{0x1402E0070, 0x140BD2C60, 0x0, 0x0};
						/* 05 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DSprite*)> Execute_Packet2DSprite{0x1402E3D10, 0x140BD6900, 0x0, 0x0};
						/* 06 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DSprite2D*)> Execute_Packet2DSprite2D{0x1402E38F0, 0x140BD64E0, 0x0, 0x0};
						/* 07 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DRSprite*)> Execute_Packet2DRSprite{0x1402E30D0, 0x140BD5CC0, 0x0, 0x0};
						/* 08 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DString*)> ExecuteOnly_Packet2DString{0x1402E4E50, 0x140BD7A40, 0x0, 0x0};
						/* 09 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DString2D*)> Execute_Packet2DString2D{0x1402E4250, 0x140BD6E40, 0x0, 0x0};
						/* 10 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DCube*)> Execute_Packet2DCube{0x1402E0640, 0x140BD3230, 0x0, 0x0};
						/* 11 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DLineStrip*)> Execute_Packet2DLineStrip{0x1402E2560, 0x140BD5150, 0x0, 0x0};
						/* 12 */ template<size_t Count> symbol<void*(Draw2DRenderer*, Packet2DTriangleStrip<Count>*)> Execute_Packet2DTriangleStrip{0x1402E4780, 0x140BD7370, 0x0, 0x0};
						/* 13 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DViewport*)> Execute_Packet2DViewport{0x1402E4AD0, 0x140BD76C0, 0x0, 0x0};
						/* 14 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DViewmap*)> Execute_Packet2DViewmap{0x1402E4A70, 0x140BD7660, 0x0, 0x0};
						/* 15 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DWorldCoords*)> Execute_Packet2DWorldCoords{0x1402E4C70, 0x140BD7860, 0x0, 0x0};
						/* 16 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DScreenCoords*)> Execute_Packet2DScreenCoords{0x1402E37D0, 0x140BD63C0, 0x0, 0x0};
						/* 17 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DMatrix*)> Execute_Packet2DMatrix{0x1402E2830, 0x140BD5420, 0x0, 0x0};
						/* 18 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DRotation*)> Execute_Packet2DRotation{0x1402E36D0, 0x140BD62C0, 0x0, 0x0};
						/* 19 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DTranslation*)> Execute_Packet2DTranslation{0x1402E4470, 0x140BD7060, 0x0, 0x0};
						/* 20 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DScale*)> Execute_Packet2DScale{0x1402E3750, 0x140BD6340, 0x0, 0x0};
						/* 21 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DBillboard*)> Execute_Packet2DBillboard{0x1402DF4B0, 0x140BD20A0, 0x0, 0x0};
						/* 22 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DPerspective*)> Execute_Packet2DPerspective{0x1402E28E0, 0x140BD54D0, 0x0, 0x0};
						/* 23 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DFlat*)> Execute_Packet2DFlat{0x1402E2320, 0x140BD4F10, 0x0, 0x0};
						/* 24 */ WEAK symbol<void*(Draw2DRenderer*, char)> SetDepthMode{0x1402E8230, 0x140BDAAC0, 0x0, 0x0};
						/* 25 */ WEAK symbol<void*(Draw2DRenderer*, char)> SetCullMode{0x1402E8130, 0x140BDA9C0, 0x0, 0x0};
						/* 26 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DTexture*)> Execute_Packet2DTexture{0x1402E43C0, 0x140BD6FB0, 0x0, 0x0};
						/* 27 */ WEAK symbol<void*(Draw2DRenderer*, char)> SetAlpha{0x1402E7E10, 0x140BDA6A0, 0x0, 0x0};
						/* 28 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DColor*)> Execute_Packet2DColor{0x1402E04B0, 0x140BD30A0, 0x0, 0x0};
						/* 29 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DStencil*)> SetStencil{0x1402E82E0, 0x140BDAB70, 0x0, 0x0};
						/* 30 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DClearStencil*)> Execute_Packet2DClearStencil{0x1402E0460, 0x140BD3050, 0x0, 0x0};
						/* 32 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DMaterial*)> Execute_Packet2DMaterial{0x1402E2710, 0x140BD5300, 0x0, 0x0};
						/* 33 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DMaterialWork*)> Execute_Packet2DMaterialWork{0x1402E27E0, 0x140BD53D0, 0x0, 0x0};
						/* 34 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DUserMatrix*)> Execute_Packet2DUserMatrix{0x1402E48F0, 0x140BD74E0, 0x0, 0x0};
						/* 38 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DPush*)> Execute_Packet2DPush{0x1402E2D00, 0x140BD58F0, 0x0, 0x0};
						/* 39 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DPop*)> Execute_Packet2DPop{0x1402E2C30, 0x140BD5820, 0x0, 0x0};
						/* 40 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DResolve*)> Execute_Packet2DResolve{0x1402E3670, 0x140BD6260, 0x0, 0x0};
						/* 41 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DCopyRenderTarget*)> Execute_Packet2DCopyRenderTarget{0x1402E05B0, 0x140BD31A0, 0x0, 0x0};
						/* 45 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DDrawIndices*)> Execute_Packet2DDrawIndices{0x1402E1F80, 0x140BD4B70, 0x0, 0x0};
						/* 46 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DUserVertexBuffer*)> Execute_Packet2DUserVertexBuffer{0x1402E4960, 0x140BD7550, 0x0, 0x0};
						/* 47 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DBeginRenderToTemporary*)> Execute_Packet2DBeginRenderToTemporary{0x1402DF350, 0x140BD1F40, 0x0, 0x0};
						/* 48 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DEndRenderToTemporary*)> Execute_Packet2DEndRenderToTemporary{0x1402E2110, 0x140BD4D00, 0x0, 0x0};
						/* 49 */ WEAK symbol<void*(Draw2DRenderer*, Packet2DSetTemporaryTexture*)> Execute_Packet2DSetTemporaryTexture{0x1402E3850, 0x140BD6440, 0x0, 0x0};
					}
				}
			}
		}
	}

	namespace luaext
	{
		WEAK symbol<void*(lua::lua_State*, const char*, __int64, __int64, __int64)> lua_bind_start{0x141A158E0, 0x141472990, 0x0, 0x0};
		WEAK symbol<void*(void*, const char*, void*, __int64, __int64, __int64)> lua_bind_closure{0x141A15690, 0x141472740, 0x0, 0x0};
		WEAK symbol<void* (void*, __int64, __int64, __int64)> lua_bind_end{0x141A15760, 0x141472810, 0x0, 0x0};
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
				WEAK symbol<Player2System> player2System{0x142A82C28, 0x141E7F578, 0x0, 0x0};

				namespace impl
				{
					namespace Player2UtilityImpl_
					{
						WEAK symbol<bool()> IsLoading{0x1409D6590, 0x1407B7E70, 0x0, 0x0};
					}
				}
			}

			namespace tool
			{
				WEAK symbol<void()> CloseSession{0x140A1A710, 0x0, 0x0, 0x0};
			}
		}

		namespace ui
		{
			namespace hud
			{
				namespace CommonDataManager_
				{
					WEAK symbol<CommonDataManager*()> GetInstance{0x140866910, 0x1405EA340, 0x0, 0x0};
					WEAK symbol<void(CommonDataManager*, const char*, int, int, int)> AnnounceLogView{0x140863C60, 0x1405E7280, 0x0, 0x0};
					WEAK symbol<bool(CommonDataManager*)> IsEndLoadingTips{0x1408670B0, 0x1405EABD0, 0x0, 0x0};
				}

				namespace AnnounceLogViewer_
				{
					WEAK symbol<char(AnnounceLogViewer*, char)> ModelInit{0x14093F280, 0x1406C9090, 0x0, 0x0};
				}
			}

			namespace menu
			{
				namespace UiCommonDataManager_
				{
					WEAK symbol<UiCommonDataManager*()> GetInstance{0x14085FD00, 0x1405E0D10, 0x0, 0x0};
					WEAK symbol<int(UiCommonDataManager*)> GetPauseMenuType{0x14085FDC0, 0x1405E0DB0, 0x0, 0x0};
					WEAK symbol<fox::ui::SoundControl*(UiCommonDataManager*)> GetSoundControl{0x1403139E0, 0x1405E0E70, 0x0, 0x0};
				}

				namespace impl
				{
					namespace MotherBaseDeviceSystemImpl_
					{
						WEAK symbol<bool()> IsDeviceOpend{0x14094A1A0, 0x1405E4320, 0x0, 0x0};
					}
				}
			}

			namespace utility
			{
				WEAK symbol<fox::StringId*(fox::StringId*, unsigned __int16, unsigned __int16, char)> GetMissionNameKey{0x1409131C0, 0x140685530, 0x0, 0x0};
				WEAK symbol<unsigned __int16()> GetCurrentLocationId{0x1409111D0, 0x140683280, 0x0, 0x0};
				WEAK symbol<unsigned __int16()> GetCurrentMissionId{0x140911210, 0x1406832C0, 0x0, 0x0};
				WEAK symbol<const char*(fox::StringId)> GetLangText{0x140912C10, 0x140684F80, 0x0, 0x0};
				WEAK symbol<fox::StringId*(fox::StringId*, const char*)> GetStringId{0x1409140E0, 0x140686430, 0x0, 0x0};

				WEAK symbol<bool(fox::ui::SoundControl*, unsigned int)> StartSound{0x14091B8E0, 0x14068D0A0, 0x0, 0x0};
				WEAK symbol<bool(fox::ui::SoundControl*, unsigned int)> StopSound{0x14091BA10, 0x14068D1D0, 0x0, 0x0};

				namespace UiUtilityImpl_
				{
					WEAK symbol<void()> CallFoxQuit{0x1409374C0, 0x1406A3F60, 0x0, 0x0};
				}
			}
		}

		namespace net
		{
			namespace ServerManager_
			{
				WEAK symbol<FobTarget*(ServerManager*)> GetFobTarget{0x1407D6470, 0x0, 0x0, 0x0};
				WEAK symbol<SessionControl*(ServerManager*)> GetSessionControl{0x1407D6810, 0x1405760C0, 0x0, 0x0};
				WEAK symbol<ServerManager*> s_instance{0x142BEFC00, 0x141F89DB0, 0x0, 0x0};
			}

			namespace FobTarget_
			{
				WEAK symbol<char(FobTarget*)> CreateHostSession{0x1407EE590, 0x0, 0x0, 0x0};
				WEAK symbol<char(FobTarget*, SessionConnectInfo*)> CreateClientSession{0x1407EE380, 0x0, 0x0, 0x0};
				WEAK symbol<void(FobTarget*, tpp::mbm::PlayerBasicInfo*, FobTargetInfo*)> ReceiveBasicInfoCommon{0x1407EE910, 0x0, 0x0, 0x0};
			}

			namespace Daemon_
			{
				WEAK symbol<Daemon*()> GetInstance{0x1407DDDB0, 0x14057B8F0, 0x0, 0x0};
			}

			namespace NetworkInfo_
			{
				WEAK symbol<NetworkInfo*> m_instance{0x142BEF840, 0x0, 0x0, 0x0};
			}

			namespace DisplayName_
			{
				WEAK symbol<void(DisplayName*)> ClearList{0x1407D78C0, 0x0, 0x0, 0x0};
				WEAK symbol<void(DisplayName*, fox::gk::FirstPartyAccount*)> AddList{0x1407D7890, 0x0, 0x0, 0x0};
				WEAK symbol<void(DisplayName*)> GetDisplayName{0x1407D7960, 0x0, 0x0, 0x0};
			}
		}

		namespace mp
		{
			namespace RulesetManager_
			{
				WEAK symbol<RulesetManager*> s_instance{0x0, 0x14204CAE8, 0x0, 0x0};
			}
		}
	}

	namespace lua
	{
		WEAK symbol<int(lua_State* s, const char* buffer, 
			size_t size, const char* name)> luaL_loadbuffer{0x141A178E0, 0x141474990, 0x0, 0x0};

		WEAK symbol<int(lua_State* s, const char*)> luaL_loadstring{0x141A17B30, 0x141474BE0, 0x0, 0x0};
		WEAK symbol<int(lua_State* s, int, int)> lua_call{0x141A10CC0, 0x14146CAB0, 0x0, 0x0};
		WEAK symbol<int(lua_State* s, int, int, int)> lua_pcall{0x141A116C0, 0x14146D4B0, 0x0, 0x0};

		WEAK symbol<void(lua_State* L, int, const char*)> lua_getfield{0x140BF2A80, 0x14146CFD0, 0x0, 0x0};
		WEAK symbol<void(lua_State* L, int, const char*)> lua_setfield{0x140BF2F30, 0x14146DBF0, 0x0, 0x0};

		WEAK symbol<int(lua_State* L, int)> lua_type{0x141A12300, 0x14146E100, 0x0, 0x0};
		WEAK symbol<const char*(lua_State* L, int)> lua_typename{0x141A12330, 0x14146E130, 0x0, 0x0};
		WEAK symbol<int(lua_State* L, int)> lua_next{0x141A11600, 0x14146D3F0, 0x0, 0x0};
		WEAK symbol<void(lua_State* L, int)> lua_settop{0x141A11F70, 0x14146DD60, 0x0, 0x0};
		WEAK symbol<void(lua_State* L, int)> lua_remove{0x141A11C20, 0x14146DA10, 0x0, 0x0};

		WEAK symbol<bool(lua_State* L, int)> lua_toboolean{0x141A120C0, 0x14146DEC0, 0x0, 0x0};
		WEAK symbol<int(lua_State* L, int)> lua_tointeger{0x141A12120, 0x14146DF20, 0x0, 0x0};
		WEAK symbol<double(lua_State* L, int)> lua_tonumber{0x141A121F0, 0x14146DFF0, 0x0, 0x0};
		WEAK symbol<const char*(lua_State* L, int idx, size_t* len)> lua_tolstring{0x141A12150, 0x14146DF50, 0x0, 0x0};

		WEAK symbol<void(lua_State* L)> lua_pushnil{0x141A11930, 0x14146D720, 0x0, 0x0};
		WEAK symbol<void(lua_State* L, const char*)> lua_pushstring{0x141A11970, 0x14146D760, 0x0, 0x0};
		WEAK symbol<void(lua_State* L, double)> lua_pushnumber{0x141A11950, 0x14146D740, 0x0, 0x0};
		WEAK symbol<void(lua_State* L, int)> lua_pushinteger{0x141A11880, 0x14146D670, 0x0, 0x0};
		WEAK symbol<void(lua_State* L, int)> lua_pushboolean{0x141C9A140, 0x14146D540, 0x0, 0x0};

#define game_lua_pop(L,n) game::lua::lua_settop(L, -(n)-1)
	}

	// Variables

	WEAK symbol<ISteamNetworking*(*)()> SteamNetworking{0x14208E138, 0x1416F1100, 0x0, 0x0};
	WEAK symbol<ISteamFriends*(*)()> SteamFriends{0x14208E110, 0x1416F10C0, 0x0, 0x0};
	WEAK symbol<ISteamMatchmaking*(*)()> SteamMatchmaking{0x0, 0x1416F10F8, 0x0, 0x0};
	WEAK symbol<ISteamUser*(*)()> SteamUser{0x14208E128, 0x1416F1108, 0x0, 0x0};

	WEAK symbol<fox::nt::impl::SessionImpl2*> s_pSession{0x142B9DFB0, 0x14209B420, 0x0, 0x0};
	WEAK symbol<fox::nt::impl::SessionIdle*> s_idleSession{0x142B9E000, 0x14209B470, 0x0, 0x0};

	WEAK symbol<mgo_matchmaking_manager> s_mgoMatchMakingManager{0x0, 0x142054160, 0x0, 0x0};

	WEAK symbol<ID3D11Device*> s_deviceD3D{0x142C6B870, 0x1420E4200, 0x0, 0x0};
	WEAK symbol<ID3D11DeviceContext*> s_immediateContextD3D{0x142C6B878, 0x1420E4208, 0x0, 0x0};

	WEAK symbol<fox::Lua*> s_instances{0x142B641F0, 0x141F28240, 0x0, 0x0};

	WEAK symbol<fox::RawKeyboardData> g_rawMouseData{0x142C6C6D8, 0x1420E5068, 0x0, 0x0};

	namespace fox
	{
		WEAK symbol<int> g_vkKeyTable{0x142C6C2D0, 0x1420E4C60, 0x0, 0x0};
	}
}
