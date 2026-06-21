#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "vars.hpp"
#include "hashes.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace ui
{
	namespace
	{
		vars::var_ptr ui_mbdvc_color;
		vars::var_ptr ui_mbdvc_text_color;

		std::unordered_set<std::uint32_t> mbdvc_color_hashes =
		{
			0x8744FF65, // cmn-mb-bg-base
			0xDF8BAAAA, // cmn-mb-frame-base
			0x3F4AB81B, // cmn-mb-plate-base
			0xF4E439F0, // unknown
			0xF7149AAA, // cmn-mb-frame-mgo
		};

		std::unordered_set<game::fox::ui::ModelNodeCommon*> mbdvc_nodes;

		utils::hook::detour graph_manager_get_palette_color_hook;
		utils::hook::detour model_node_common_destructor_hook;

		__int64 graph_manager_get_palette_color_stub(void* a1, unsigned int hash, game::fox::Color* color)
		{
			if (mbdvc_color_hashes.contains(hash))
			{
				const auto mb_color = ui_mbdvc_color->current.get_color();
				color->values[0] = mb_color.r;
				color->values[1] = mb_color.g;
				color->values[2] = mb_color.b;
				color->values[3] = mb_color.a;
				return 1;
			}

			return graph_manager_get_palette_color_hook.invoke<__int64>(a1, hash, color);
		}

		void set_node_color(game::fox::ui::ModelNodeCommon* node, const vars::color_t& var_color)
		{
			const auto uix_utility = game::fox::uix::impl::GetUixUtilityToFeedQuarkEnvironment();

			game::fox::Color color{};
			color.values[0] = var_color.r;
			color.values[1] = var_color.g;
			color.values[2] = var_color.b;
			color.values[3] = var_color.a;

			if (game::environment::is_tpp())
			{
				uix_utility->__vftable->tpp.SetColorRGB3(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(node), &color);
			}
			else
			{
				uix_utility->__vftable->mgo.SetColorRGB3(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(node), &color);
			}
		}

		void update_bg_nodes()
		{
			const auto color = ui_mbdvc_color->current.get_color();
			for (auto& node : mbdvc_nodes)
			{
				set_node_color(node, color);
			}
		}

		void add_model_node_common(game::fox::ui::ModelNodeCommon* node, unsigned int color_hash)
		{
			const auto uix_utility = game::fox::uix::impl::GetUixUtilityToFeedQuarkEnvironment();

			if (game::environment::is_tpp())
			{
				if (uix_utility->__vftable->tpp.GetAlpha1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(node)) == 0.f)
				{
					return;
				}
			}
			else
			{
				if (uix_utility->__vftable->mgo.GetAlpha1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(node)) == 0.f)
				{
					return;
				}
			}

			if (mbdvc_color_hashes.contains(color_hash))
			{
				mbdvc_nodes.insert(node);
			}
		}

		void model_node_common_destructor_stub(game::fox::ui::ModelNodeCommon* node)
		{
			mbdvc_nodes.erase(node);
			model_node_common_destructor_hook.invoke<void>(node);
		}

		void model_node_create_common_stub(utils::hook::assembler& a)
		{
			a.shufps(xmm2, xmm1, 0x55);
			a.movss(xmm1, dword_ptr(rsp, 0x20));
			a.call(SELECT_VALUE(0x141DBE160, 0x140E2F500, 0x0, 0x0));

			a.pushad64();
			a.movzx(eax, word_ptr(rbx, 0x4A));
			a.mov(edx, dword_ptr(r14, eax, 2));
			a.mov(rcx, rdi);
			a.call_aligned(add_model_node_common);
			a.popad64();

			a.jmp(SELECT_VALUE(0x141DBD568, 0x140E2E908, 0x0, 0x0));
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			vars::color_t color{};
			color.r = 0.05f;
			color.g = 0.25f;
			color.b = 0.35f;
			color.a = 1.f;

			ui_mbdvc_color = vars::register_color("ui_mb_color", color, vars::var_flag_saved, "idroid ui background color");
			ui_mbdvc_color->set_callback = update_bg_nodes;

			graph_manager_get_palette_color_hook.create(SELECT_VALUE(0x141DAB770, 0x140E1C6D0, 0x0, 0x0), graph_manager_get_palette_color_stub);
			utils::hook::jump(SELECT_VALUE(0x141DBD559, 0x140E2E8F9, 0x0, 0x0), utils::hook::assemble(model_node_create_common_stub), true);
			model_node_common_destructor_hook.create(SELECT_VALUE(0x141DBD320, 0x140E2E6C0, 0x0, 0x0), model_node_common_destructor_stub);
		}

		void start() override
		{

		}
	};
}

REGISTER_COMPONENT(ui::component)
