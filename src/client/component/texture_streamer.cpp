#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "vars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace texture_streamer
{
	namespace
	{
		vars::var_ptr r_increase_texture_memory;

		unsigned __int64 get_available_storage_memory()
		{
			constexpr const auto cap = 0xFFFFFFFFull;
			const auto size = game::s_adapterDXGIDesc->DedicatedVideoMemory + 
				game::s_adapterDXGIDesc->DedicatedSystemMemory;

			if (size > cap)
			{
				return cap;
			}

			return size;
		}

		utils::hook::detour request_texture_storage_configuration_hook;
		char request_texture_storage_configuration_stub(void* a1, unsigned __int64 a2)
		{
			*reinterpret_cast<unsigned int*>(a2) = static_cast<unsigned int>(get_available_storage_memory());
			return request_texture_storage_configuration_hook.invoke<char>(a1, a2);
		}

		utils::hook::detour texture_storage_manager_alloc_hook;
		unsigned int texture_storage_manager_alloc_stub(void* manager, unsigned int size, int type)
		{
			const auto result = texture_storage_manager_alloc_hook.invoke<unsigned int>(manager, size, type);
			if (result != 0xFFFFFFFF || type != 0)
			{
				return result;
			}

			return texture_storage_manager_alloc_hook.invoke<unsigned int>(manager, size, 2);
		}
		
		void assembler_get_memory(utils::hook::assembler& a)
		{
			a.push(rax);
			a.pushad64();
			a.call_aligned(get_available_storage_memory);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);
		}

		void texture_streamer_memory_reservation_stub(utils::hook::assembler& a)
		{
			assembler_get_memory(a);
			a.mov(dword_ptr(rcx, 0x30A64), eax);
			a.mov(dword_ptr(rcx, 0x30A58), 0x10680000);
			a.jmp(SELECT_VALUE_LANG(0x140B23DF9, 0x0));
		}

		void texture_streamer_dispatch_stub(utils::hook::assembler& a)
		{
			assembler_get_memory(a);
			a.mov(edi, eax);
			a.jmp(SELECT_VALUE_LANG(0x140B2066D, 0x0));
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			r_increase_texture_memory = vars::register_bool("r_increase_texture_memory", true,
				vars::var_flag_saved, "increase texture streamer memory");
		}

		void start() override
		{
			if (!r_increase_texture_memory->current.enabled())
			{
				return;
			}

			request_texture_storage_configuration_hook.create(SELECT_VALUE(0x1402A91D0, 0x140B9F920, 0x0, 0x0),
				request_texture_storage_configuration_stub);

			texture_storage_manager_alloc_hook.create(SELECT_VALUE(0x1402A6D40, 0x140B9D580, 0x0, 0x0),
				texture_storage_manager_alloc_stub);

			if (game::environment::is_tpp())
			{
				utils::hook::jump(SELECT_VALUE_LANG(0x1402A8F60, 0x0), get_available_storage_memory);
				utils::hook::set<std::uint32_t>(SELECT_VALUE_LANG(0x14021A772, 0x0) + 1, 0xFFFFFFFF);
				utils::hook::set<std::uint32_t>(SELECT_VALUE_LANG(0x14021DF74, 0x0) + 1, 0xFFFFFFFF);
				utils::hook::set<std::uint8_t>(SELECT_VALUE(0x14021A7A0, 0x140B20660, 0x0), 0xEB);
				utils::hook::nop(SELECT_VALUE_LANG(0x14021A797, 0x0), 4);
			}
			else
			{
				utils::hook::jump(SELECT_VALUE_LANG(0x140B23DE5, 0x0), 
					utils::hook::assemble(texture_streamer_memory_reservation_stub), true);

				utils::hook::jump(SELECT_VALUE_LANG(0x140B20644, 0x0),
					utils::hook::assemble(texture_streamer_dispatch_stub), true);
			}

			utils::hook::set<std::uint8_t>(SELECT_VALUE(0x1402216E0, 0x140B274E0, 0x0) + 6, 0x20);
			utils::hook::set<std::uint8_t>(SELECT_VALUE(0x1402217A0, 0x140B275A0, 0x0) + 6, 0x20);
		}
	};
}

REGISTER_COMPONENT(texture_streamer::component)
