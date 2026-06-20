#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "vars.hpp"

#include <utils/hook.hpp>

namespace renderer_vars
{
	namespace
	{
		vars::var_ptr var_r_draw2d;
		vars::var_ptr var_r_fog;
		vars::var_ptr var_r_fxaa;

		utils::hook::detour gr_plugin_2d_main_exec_hook;

		bool get_fog_enabled()
		{
			return var_r_fog->current.enabled();
		}

		bool get_fxaa_enabled()
		{
			switch (var_r_fxaa->current.get_int())
			{
			case 0:
				return false;
			case 1:
				return true;
			}

			return *reinterpret_cast<int*>(SELECT_VALUE(0x142B79984, 0x142074DA4, 0x142B79984, 0x142074E04)) == 1;
		}

		void volumetric_fog_manager_update_stub(utils::hook::assembler& a)
		{
			const auto fog_disabled = a.new_label();
			const auto fog_enabled = a.new_label();

			a.push(rax);
			a.pushad64();
			a.call_aligned(get_fog_enabled);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);

			a.mov(qword_ptr(rdi, 0x18), rdx);
			a.and_(sil, al);
			a.test(sil, sil);
			a.jnz(fog_enabled);

			a.jmp(SELECT_VALUE(0x1406BAF39, 0x14045BFD9, 0x1406B9C29, 0x14045B5E9));

			a.bind(fog_enabled);
			a.jmp(SELECT_VALUE(0x1406BAFD8, 0x14045C078, 0x1406B9CC8, 0x14045B688));
		}

		void gr_plugin_2d_main_exec_stub(void* a1, void* a2, void* a3)
		{
			if (!var_r_draw2d->current.enabled())
			{
				return;
			}

			gr_plugin_2d_main_exec_hook.invoke<void>(a1, a2, a3);
		}

		void gr_plugin_fxaa_main_exec_stub(utils::hook::assembler& a)
		{
			const auto disabled = a.new_label();

			a.push(rax);
			a.pushad64();
			a.call_aligned(get_fxaa_enabled);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);

			a.test(al, al);
			a.jz(disabled);

			a.mov(rcx, qword_ptr(rcx, 0x68));
			a.mov(rdx, r8);
			a.mov(rax, qword_ptr(rcx));
			a.jmp(qword_ptr(rax, 0x20));

			a.bind(disabled);
			a.rep().ret();
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			var_r_fog = vars::register_bool("r_fog", true, vars::var_flag_saved, "enable fog");
			var_r_fxaa = vars::register_int("r_fxaa", 2, 0, 2, vars::var_flag_saved, "enable fxaa (0 = disabled, 1 = enabled, 2 = unchanged)");
			var_r_draw2d = vars::register_bool("r_draw2d", true, vars::var_flag_saved, "draw 2d");

			utils::hook::jump(SELECT_VALUE(0x1406BAF2C, 0x14045BFCC, 0x1406B9C1C, 0x14045B5DC), utils::hook::assemble(volumetric_fog_manager_update_stub), true);
			utils::hook::jump(SELECT_VALUE(0x1402155B5, 0x140B1BCA5, 0x143A52E75, 0x148D8B305), utils::hook::assemble(gr_plugin_fxaa_main_exec_stub), true);

			gr_plugin_2d_main_exec_hook.create(SELECT_VALUE(0x1401C0AD0, 0x140AB0980, 0x143792570, 0x148B41FB0), gr_plugin_2d_main_exec_stub);
		}
	};
}

REGISTER_COMPONENT(renderer_vars::component)
