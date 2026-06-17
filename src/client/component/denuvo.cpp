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
		void sub_14501F770_stub(__int64 a1)
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

		__int64 sub_14472D030_stub(__int64 a1)
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

		void sub_148CEEF90_stub(__int64 a1)
		{
			auto v1 = a1;
			auto v2 = 16;
			auto v3 = a1 + 128;
			auto v4 = a1 + 160;

			do
			{
				auto v5 = *reinterpret_cast<__m128*>(v3);
				auto v6 = *reinterpret_cast<__m128*>(v3 + 16);

				if (v2 >= 15)
				{
					auto v7 = v4;
					auto v8 = (v4 + 16);
					auto v9 = 15;

					do
					{
						v5 = _mm_min_ps(v5, *reinterpret_cast<__m128*>(v7));
						v6 = _mm_max_ps(v6, *reinterpret_cast<__m128*>(v8));
						v7 += 32;
						v8 += 32;
						--v9;
					} while (v9);
				}
				*reinterpret_cast<__m128*>(v1) = v5;
				v2 += 16;
				*reinterpret_cast<__m128*>(v1 + 16) = v6;
				v1 += 32;
				v3 += 512;
				v4 += 512;
			} while (v2 < 0x50);
		}

		__int64 sub_148552EA0_stub(__int64 a1, __int64 a2)
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

		__int64 sub_147032870_stub(game::fox::StringId id)
		{
			const auto util = game::fox::uix::impl::GetUixUtilityToFeedQuarkEnvironment();
			const auto vtable = reinterpret_cast<__int64*>(util->__vftable);
			return utils::hook::invoke<__int64>(vtable[234], id);
		}

		void restore_funcs()
		{
			utils::hook::jump(SELECT_VALUE(0x14501F770, 0x0, 0x0, 0x0), sub_14501F770_stub); // CAkPBI::CalculateMutedEffectiveVolume
			utils::hook::jump(SELECT_VALUE(0x14472D030, 0x0, 0x0, 0x0), sub_14472D030_stub); // fox::gr::dg::VertexBufferResourceDx11::GetBufferAddress
			utils::hook::jump(SELECT_VALUE(0x148CEEF90, 0x0, 0x0, 0x0), sub_148CEEF90_stub); // fox::geo::GeoSweepBlock::BuildBounding16
			utils::hook::jump(SELECT_VALUE(0x148552EA0, 0x0, 0x0, 0x0), sub_148552EA0_stub); // fox::uix::impl::PrefabListActImpl::GetInterface
			utils::hook::jump(SELECT_VALUE(0x147032870, 0x0, 0x0, 0x0), sub_147032870_stub); // tpp::ui::utility::GetLangText

			// todo:
			// 0x144C8DB30 CAkRTPCMgr::AkRTPCEntry::ApplyRTPCValue
			// 0x1469B4E10 tpp::ui::hud::CommonDataManager::IsEndLoadingTips
			// 0x1444AAFE0 fox::gr::DirectionalLightSource::SetShadowRangeExtra
			// 0x146512620 tpp::ef::atmosphere::impl::DirectionalLightController::UpdateGrDirectionalLightShadowParameters
			// 0x1437A3420 fox::Scene::GetDefaultScene
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
