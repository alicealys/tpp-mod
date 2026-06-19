#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "vars.hpp"
#include "scheduler.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/io.hpp>

namespace denuvo
{
	namespace
	{
		void calculate_muted_effective_volume_stub(__int64 a1)
		{
			auto result = *reinterpret_cast<unsigned __int64*>(a1 + 48);
			auto v2 = 1.0f;
			auto i = result + 24ull * static_cast<unsigned __int64>(*reinterpret_cast<unsigned int*>(a1 + 56));

			for (; result != i; result += 24)
			{
				v2 = v2 * *reinterpret_cast<float*>(result + 16);
			}

			auto v4 = (v2 * *reinterpret_cast<float*>(a1 + 288)) * *reinterpret_cast<float*>(a1 + 292);
			if (v4 <= 0.0f)
			{
				v4 = 0.0f;
			}

			*reinterpret_cast<float*>(a1 + 196) = v4;
			*reinterpret_cast<float*>(a1 + 192) = *reinterpret_cast<float*>(a1 + 276) + *reinterpret_cast<float*>(a1 + 64);
		}

		__int64 vertex_buffer_resource_get_buffer_address_stub(__int64 a1)
		{
			if (*reinterpret_cast<int*>(a1 + 36) == 1 || *reinterpret_cast<int*>(a1 + 36) == 3)
			{
				return *reinterpret_cast<__int64*>(a1 + 40) + *reinterpret_cast<__int64*>(a1 + 48);
			}
			else
			{
				return 0;
			}
		}

		void geo_sweep_block_build_bounding_16_stub(__int64 a1)
		{
			static const auto stub = utils::hook::assemble([](utils::hook::assembler& a)
			{
				static std::uint8_t bytes[] =
				{
					0x49, 0x89, 0xC9,
					0xB8, 0x10, 0x00, 0x00, 0x00,
					0x4C, 0x8D, 0x99, 0x80, 0x00, 0x00, 0x00,
					0x48, 0x8D, 0x99, 0xA0, 0x00, 0x00, 0x00,
					0x0F, 0x1F, 0x44, 0x00, 0x00,
					0x41, 0x0F, 0x28, 0x03,
					0x41, 0x0F, 0x28, 0x4B, 0x10,
					0x44, 0x8D, 0x90, 0xF1, 0xFF, 0xFF, 0xFF,
					0x41, 0x39, 0xC2,
					0x73, 0x23,
					0x89, 0xC1,
					0x48, 0x89, 0xDA,
					0x4C, 0x8D, 0x43, 0x10,
					0x44, 0x29, 0xD1,
					0x41, 0x89, 0xCA,
					0x0F, 0x5D, 0x02,
					0x41, 0x0F, 0x5F, 0x08,
					0x48, 0x8D, 0x52, 0x20,
					0x4D, 0x8D, 0x40, 0x20,
					0x49, 0xFF, 0xCA,
					0x75, 0xEC,
					0x41, 0x0F, 0x29, 0x01,
					0x83, 0xC0, 0x10,
					0x41, 0x0F, 0x29, 0x49, 0x10,
					0x49, 0x83, 0xC1, 0x20,
					0x49, 0x81, 0xC3, 0x00, 0x02, 0x00, 0x00,
					0x48, 0x81, 0xC3, 0x00, 0x02, 0x00, 0x00,
					0x83, 0xF8, 0x50,
					0x72, 0xA5,
					0x48, 0x8B, 0x5C, 0x24, 0x08,
					0xC3,
				};

				a.embed(bytes, sizeof(bytes));
			});

			utils::hook::invoke<void>(stub, a1);
		}

		__int64 prefab_list_act_impl_get_interface_stub(__int64 a1, __int64 a2)
		{
			if (a2 == 0x49C8FE468F26LL && a1 && a1 != -32)
			{
				return a1 + 40;
			}
			else
			{
				return 0;
			}
		}

		__int64 get_lang_text_stub(game::fox::StringId id)
		{
			const auto util = game::fox::uix::impl::GetUixUtilityToFeedQuarkEnvironment();
			const auto vtable = reinterpret_cast<__int64*>(util->__vftable);
			return utils::hook::invoke<__int64>(vtable[234], util, id);
		}

		__int64 geo_sweep_block_add_data_stub(__int64 a1, __int64 a2, __int64 a3)
		{
			static const auto stub = utils::hook::assemble([](utils::hook::assembler& a)
			{
				static std::uint8_t bytes[] =
				{
					0x53,
					0x56,
					0x57,
					0x48, 0x83, 0xEC, 0x50,
					0x48, 0x89, 0xD6,
					0x48, 0x89, 0xCF,
					0x31, 0xDB,
					0x48, 0x8D, 0x81, 0x80, 0x08, 0x00, 0x00,
					0x48, 0x83, 0x38, 0x00,
					0x74, 0x10,
					0xFF, 0xC3,
					0x48, 0x83, 0xC0, 0x08,
					0x83, 0xFB, 0x40,
					0x72, 0xEF,
					0x83, 0xC8, 0xFF,
					0xEB, 0x6B,
					0x48, 0x8D, 0x4C, 0x24, 0x20,
					0x41, 0xFF, 0xD0,
					0x8D, 0x0C, 0x1B,
					0x89, 0xCA,
					0x44, 0x8D, 0x41, 0x01,
					0x0F, 0x28, 0x00,
					0x0F, 0x28, 0x48, 0x10,
					0x48, 0x83, 0xC2, 0x08,
					0x48, 0x01, 0xD2,
					0x89, 0xD8,
					0x49, 0x83, 0xC0, 0x08,
					0xC1, 0xE8, 0x04,
					0x4D, 0x01, 0xC0,
					0x8D, 0x0C, 0x00,
					0x0F, 0x29, 0x04, 0xD7,
					0x89, 0xC8,
					0x48, 0x01, 0xC0,
					0x42, 0x0F, 0x29, 0x0C, 0xC7,
					0x0F, 0x28, 0x04, 0xC7,
					0x0F, 0x5D, 0x04, 0xD7,
					0x8D, 0x51, 0x01,
					0x48, 0x01, 0xD2,
					0x0F, 0x29, 0x04, 0xC7,
					0x89, 0xD8,
					0x0F, 0x28, 0x04, 0xD7,
					0x42, 0x0F, 0x5F, 0x04, 0xC7,
					0x0F, 0x29, 0x04, 0xD7,
					0x48, 0x89, 0xB4, 0xDF, 0x80, 0x08, 0x00, 0x00,
					0xFF, 0x87, 0x88, 0x0A, 0x00, 0x00,
					0x48, 0x83, 0xC4, 0x50,
					0x5F,
					0x5E,
					0x5B,
					0xC3,
				};

				a.embed(bytes, sizeof(bytes));
			});

			return utils::hook::invoke<__int64>(stub, a1, a2, a3);
		}

		void* get_default_scene_stub()
		{
			static const auto ptr1 = *reinterpret_cast<void**>(SELECT_VALUE(0x142B64480, 0x0, 0x0, 0x0));
			static const auto fn1 = SELECT_VALUE(0x140083A30, 0x0, 0x0, 0x0);
			static const auto fn2 = SELECT_VALUE(0x1400F5DE0, 0x0, 0x0, 0x0);

			if (ptr1 == nullptr)
			{
				return nullptr;
			}

			const auto ptr2 = utils::hook::invoke<void*>(fn1, ptr1);
			if (ptr2 == nullptr)
			{
				return nullptr;
			}

			const auto ptr3 = *utils::hook::invoke<__int64*>(fn2, ptr2);
			if (ptr3 == 0)
			{
				return nullptr;
			}

			return *reinterpret_cast<void**>(ptr3 + 80);
		}

		__int64 apply_rtpc_value_stub(__int64 a1, __int64 a2, float a3, __int64 a4, bool a5)
		{
			static const auto fn1 = SELECT_VALUE(0x140356670, 0x0, 0x0, 0x0);
			static const auto fn2 = SELECT_VALUE(0x1403543E0, 0x0, 0x0, 0x0);
			static const auto fn3 = SELECT_VALUE(0x1403542B0, 0x0, 0x0, 0x0);

			utils::hook::invoke<void>(fn1, a1, a2, a3, a4);

			if (a5)
			{
				if (a2)
				{
					utils::hook::invoke<__int64>(fn2, a1 + 24, a4);
				}

				return 1;
			}
			else if (a2)
			{
				*reinterpret_cast<float*>(a2 + 8) = a3;
				return 1;
			}
			else
			{
				auto v9 = utils::hook::invoke<__int64>(fn3, a1 + 24, a4);
				if (v9)
				{
					*reinterpret_cast<float*>(v9 + 8) = a3;
					return 1;
				}
				else
				{
					return 2;
				}
			}
		}

		void scene_dequeue_stub(__int64 a1, __int64 a2)
		{
			static const auto fn1 = SELECT_VALUE(0x14002DAA0, 0x0, 0x0, 0x0);
			static const auto fn2 = SELECT_VALUE(0x1401B5E20, 0x0, 0x0, 0x0);
			static const auto fn3 = SELECT_VALUE(0x1401B5330, 0x0, 0x0, 0x0);
			static const auto ptr1 = *reinterpret_cast<__int64*>(SELECT_VALUE(0x142B73290, 0x0, 0x0, 0x0));

			if (a2)
			{
				auto ptr2 = utils::hook::invoke<__int64>(fn1);
				if (ptr2)
				{
					if (ptr1)
					{
						utils::hook::invoke<void>(fn2, ptr1, *reinterpret_cast<void**>(ptr2 + 8), a2);
					}
				}
				else
				{
					utils::hook::invoke<void>(fn3, a1, a2);
				}
			}
		}

		void set_shadow_range_extra_stub(__int64 a1, float a2)
		{
			static const auto fn1 = SELECT_VALUE(0x14026EB00, 0x0, 0x0, 0x0);
			*reinterpret_cast<float*>(a1 + 124) = a2;

			auto a1_1 = *reinterpret_cast<__m128**>(a1 + 64);
			auto a2_1 = *reinterpret_cast<unsigned int*>(a1 + 116);
			auto a3_1 = a2;
			auto a4_1 = *reinterpret_cast<float*>(a1 + 128);
			auto a5_1 = *reinterpret_cast<float*>(a1 + 132);
			auto a6_1 = *reinterpret_cast<unsigned int*>(a1 + 136);

			utils::hook::invoke<void>(fn1, a1_1, a2_1, a3_1, a4_1, a5_1, a6_1);
		}

		char is_end_loading_tips_stub(__int64 a1)
		{
			static const auto fn1 = SELECT_VALUE(0x140934120, 0x0, 0x0, 0x0);
			static const auto fn2 = SELECT_VALUE(0x140934110, 0x0, 0x0, 0x0);

			auto ptr1 = *reinterpret_cast<__int64*>(a1 + 3728);
			if (ptr1 != 0 && utils::hook::invoke<char>(fn1, ptr1, 0) && 
				utils::hook::invoke<char>(fn2, *reinterpret_cast<__int64*>(a1 + 3728), 0))
			{
				return *reinterpret_cast<char*>(a1 + 5510);
			}
			else
			{
				return 1;
			}
		}

		void set_cloud_amount_threshold_stub(__int64 a1, float a2, int a3, int a4)
		{
			static const auto fn1 = SELECT_VALUE(0x14026D350, 0x0, 0x0, 0x0);

			auto a1_1 = *reinterpret_cast<__m128**>(a1 + 64);
			auto a2_1 = a1 + 96;
			auto a3_1 = *reinterpret_cast<float*>(a1 + 140);
			auto a4_1 = *reinterpret_cast<float*>(a1 + 144);
			auto a5_1 = a2;

			*reinterpret_cast<float*>(a1 + 148) = a2;

			utils::hook::invoke<void>(fn1, a1_1, a2_1, a3_1, a4_1, a5_1);
		}

		void set_shadow_bias_stub(__int64 a1, float a2)
		{
			static const auto fn1 = SELECT_VALUE(0x14026EB00, 0x0, 0x0, 0x0);

			auto a1_1 = *reinterpret_cast<__m128**>(a1 + 64);
			auto a2_1 = *reinterpret_cast<unsigned int*>(a1 + 116);
			auto a3_1 = *reinterpret_cast<float*>(a1 + 124);
			auto a4_1 = *reinterpret_cast<float*>(a1 + 128);
			auto a5_1 = *reinterpret_cast<float*>(a1 + 132);
			auto a6_1 = *reinterpret_cast<float*>(a1 + 136);

			*reinterpret_cast<float*>(a1 + 108) = a2;

			utils::hook::invoke<void>(fn1, a1_1, a2_1, a3_1, a4_1, a5_1, a6_1);
		}

		__int64 update_gr_directional_light_shadow_params_stub(__int64 a1, __int64 a2)
		{
			static const auto fn1 = SELECT_VALUE(0x1402624A0, 0x0, 0x0, 0x0);
			static const auto fn2 = SELECT_VALUE(0x140262590, 0x0, 0x0, 0x0);
			static const auto fn3 = set_shadow_range_extra_stub;
			static const auto fn4 = SELECT_VALUE(0x140262620, 0x0, 0x0, 0x0);

			static const auto fn5 = SELECT_VALUE(0x1402624C0, 0x0, 0x0, 0x0);
			static const auto fn6 = set_shadow_bias_stub;
			static const auto fn7 = SELECT_VALUE(0x140262850, 0x0, 0x0, 0x0);

			static const auto fn8 = SELECT_VALUE(0x140262130, 0x0, 0x0, 0x0);
			static const auto fn9 = SELECT_VALUE(0x1402620C0, 0x0, 0x0, 0x0);
			static const auto fn10 = SELECT_VALUE(0x14448CF50, 0x0, 0x0, 0x0);
			static const auto fn11 = set_cloud_amount_threshold_stub;

			auto v3 = (*reinterpret_cast<unsigned int*>(a1 + 204) & 2) != 0;
			auto ptr1 = *reinterpret_cast<__int64*>(a1 + 128);

			utils::hook::invoke<void>(fn1, ptr1, v3);
			utils::hook::invoke<void>(fn2, ptr1, *reinterpret_cast<float*>(a1 + 156));
			utils::hook::invoke<void>(fn3, ptr1, *reinterpret_cast<float*>(a1 + 160));
			utils::hook::invoke<void>(fn4, ptr1, *reinterpret_cast<float*>(a1 + 164));

			*reinterpret_cast<unsigned int*>(ptr1 + 112ull) = *reinterpret_cast<unsigned int*>(a1 + 168);
			utils::hook::invoke<void>(fn5, ptr1, *reinterpret_cast<float*>(a1 + 172));
			utils::hook::invoke<void>(fn6, ptr1, *reinterpret_cast<float*>(a1 + 176));
			utils::hook::invoke<void>(fn7, ptr1, *reinterpret_cast<float*>(a1 + 152));
			
			auto v4 = (*reinterpret_cast<unsigned int*>(a1 + 204) & 4) != 0 && *reinterpret_cast<float*>(a1 + 184) > 0.000001f;
			utils::hook::invoke<void>(fn8, ptr1, v4);
			utils::hook::invoke<void>(fn9, ptr1, a1 + 180, a1 + 184);
			utils::hook::invoke<void>(fn10, ptr1, *reinterpret_cast<float*>(a1 + 192));
			utils::hook::invoke<void>(fn11, ptr1, *reinterpret_cast<int*>(a1 + 196));

			return 1;
		}

		void restore_funcs()
		{
			utils::hook::jump(SELECT_VALUE(0x14501F770, 0x0, 0x0, 0x0), calculate_muted_effective_volume_stub);          // CAkPBI::CalculateMutedEffectiveVolume
			utils::hook::jump(SELECT_VALUE(0x14472D030, 0x0, 0x0, 0x0), vertex_buffer_resource_get_buffer_address_stub); // fox::gr::dg::VertexBufferResourceDx11::GetBufferAddress
			utils::hook::jump(SELECT_VALUE(0x148CEEF90, 0x0, 0x0, 0x0), geo_sweep_block_build_bounding_16_stub);         // fox::geo::GeoSweepBlock::BuildBounding16
			utils::hook::jump(SELECT_VALUE(0x148552EA0, 0x0, 0x0, 0x0), prefab_list_act_impl_get_interface_stub);        // fox::uix::impl::PrefabListActImpl::GetInterface
			utils::hook::jump(SELECT_VALUE(0x147032870, 0x0, 0x0, 0x0), get_lang_text_stub);                             // tpp::ui::utility::GetLangText
			utils::hook::jump(SELECT_VALUE(0x148CE07F0, 0x0, 0x0, 0x0), geo_sweep_block_add_data_stub);                  // fox::geo::GeoSweepBlock::AddData
			utils::hook::jump(SELECT_VALUE(0x1437A3420, 0x0, 0x0, 0x0), get_default_scene_stub);                         // fox::Scene::GetDefaultScene
			utils::hook::jump(SELECT_VALUE(0x144C8DB30, 0x0, 0x0, 0x0), apply_rtpc_value_stub);                          // CAkRTPCMgr::AkRTPCEntry::ApplyRTPCValue
			utils::hook::jump(SELECT_VALUE(0x143B294F0, 0x0, 0x0, 0x0), scene_dequeue_stub);                             // fox::gr::Scene::Dequeue
			utils::hook::jump(SELECT_VALUE(0x1444AAFE0, 0x0, 0x0, 0x0), set_shadow_range_extra_stub);                    // fox::gr::DirectionalLightSource::SetShadowRangeExtra
			utils::hook::jump(SELECT_VALUE(0x1469B4E10, 0x0, 0x0, 0x0), is_end_loading_tips_stub);	                     // tpp::ui::hud::CommonDataManager::IsEndLoadingTips
			utils::hook::jump(SELECT_VALUE(0x146512620, 0x0, 0x0, 0x0), update_gr_directional_light_shadow_params_stub); // tpp::ef::atmosphere::impl::DirectionalLightController::UpdateGrDirectionalLightShadowParameters
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			// restore obfuscated functions that cause lag
			restore_funcs();
		}
	};
}

REGISTER_COMPONENT(denuvo::component)
