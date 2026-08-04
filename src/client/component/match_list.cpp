#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "vars.hpp"
#include "matchmaking.hpp"
#include "custom_maps.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace match_list
{
	namespace
	{
		vars::var_ptr var_match_host_comment;

		void append_host_comment_stub(void* a1, __int64 a2, unsigned int a3, unsigned char a4)
		{
			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			const auto entry_ptr = a2 - 86;
			const auto entry_unk = *reinterpret_cast<__int64*>(entry_ptr + 6);
			const auto lobby_info_ptr = entry_unk - 200;
			const auto lobby_index = *reinterpret_cast<int*>(lobby_info_ptr + 40);

			game::steam_id lobby_id{};
			steam_matchmaking->__vftable->GetLobbyByIndex(steam_matchmaking, &lobby_id, lobby_index);

			const auto host_comment_str = steam_matchmaking->__vftable->GetLobbyData(steam_matchmaking, lobby_id, "host_comment_string");
			if (host_comment_str[0] != 0)
			{
				strncpy_s(reinterpret_cast<char*>(a2), 0x100, host_comment_str, _TRUNCATE);
			}
			else
			{
				utils::hook::invoke<void>(SELECT_VALUE_LANG(0x1413B8BB0, 0x0), a1, a2, a3, a4);
			}
		}

		void match_list_map_name_stub1(utils::hook::assembler& a)
		{
			const auto unknown_map = a.new_label();

			a.xor_(r8, r8);
			a.movsxd(rax, dword_ptr(r14, 8));
			a.cmp(rax, 8);
			a.mov(r8, 0x15683A723096347D);
			a.jae(unknown_map);

			a.xor_(ecx, ecx);
			a.cmp(byte_ptr(r14, 0x15), cl);
			a.setnz(cl);
			a.lea(r8, qword_ptr(rcx, rax, 1));
			a.mov(r8, qword_ptr(rbp, r8, 3, 0x30));

			a.bind(unknown_map);
			a.mov(rcx, rsi);
			a.mov(rax, qword_ptr(rsi));
			a.call(qword_ptr(rax, 0x520));

			a.jmp(SELECT_VALUE_LANG(0x1413A6251, 0x0));
		}

		void match_list_map_name_stub2(utils::hook::assembler& a)
		{
			const auto unknown_map = a.new_label();

			a.movzx(edx, rcx);

			a.cmp(rcx, 8);
			a.jae(unknown_map);
			a.movzx(edx, byte_ptr(rsp, rcx, 0, 0x3F0));

			a.bind(unknown_map);
			a.mov(rcx, rdi);
			a.mov(dword_ptr(rbx), edx);

			a.jmp(SELECT_VALUE_LANG(0x1413BC1A6, 0x0));
		}

		static __int64 builtin_map_names[] =
		{
			0x42B62FB06FA4,
			0xCA4C57ED2A62,
			0x6801CDA06B50,
			0x90117FAF326F,
			0xF2509DF38B5C,
			0x0C78ACCFD99E,
			0xD4212511174D,
			0x18AC1D57A569,
		};

		auto did_match_list = false;

		const char* get_custom_map_name(__int64 entry)
		{
			did_match_list = true;

			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			const auto entry_unk = *reinterpret_cast<__int64*>(entry + 6);
			const auto lobby_info_ptr = entry_unk - 200;
			const auto lobby_index = *reinterpret_cast<int*>(lobby_info_ptr + 40);

			const auto map_id = *reinterpret_cast<int*>(entry - 18);
			if (map_id >= 8)
			{
				game::steam_id lobby_id{};
				steam_matchmaking->__vftable->GetLobbyByIndex(steam_matchmaking, &lobby_id, lobby_index);

				static char buffer[64]{};
				const auto name = steam_matchmaking->__vftable->GetLobbyData(steam_matchmaking, lobby_id, "map_name");
				strncpy_s(buffer, sizeof(buffer), name, _TRUNCATE);
				return buffer;
			}
			else
			{
				game::fox::StringId id{};
				id.id = builtin_map_names[map_id];
				return game::tpp::ui::utility::GetLangText(id);
			}
		}

		void match_list_map_name_stub3(utils::hook::assembler& a)
		{
			a.mov(rcx, qword_ptr(rsp, 0x38));

			a.push(rax);
			a.pushad64();
			a.call_aligned(get_custom_map_name);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);

			a.jmp(SELECT_VALUE_LANG(0x1413BA2BA, 0x0));
		}

		void match_list_map_name_stub4(utils::hook::assembler& a)
		{
			const auto unknown_map = a.new_label();

			a.movzx(eax, byte_ptr(rsi, 0xD));
			a.cmp(eax, 8);
			a.jae(unknown_map);

			a.movsx(eax, byte_ptr(rsp, rax, 0, 0x20));

			a.bind(unknown_map);
			a.mov(dword_ptr(rdi), eax);
			a.movsx(edx, byte_ptr(rsi, 0xC));

			a.jmp(SELECT_VALUE_LANG(0x1413BB23F, 0x0));
		}

		void match_list_map_name_stub5(utils::hook::assembler& a)
		{
			const auto unknown_map = a.new_label();

			a.pop(rax);

			a.movzx(ecx, rdx);
			a.cmp(rdx, 8);

			a.jae(unknown_map);
			a.movzx(ecx, byte_ptr(rsp, rdx, 0, 0xF0));

			a.bind(unknown_map);
			a.mov(byte_ptr(rax, -0x1E), cl);
			a.setnz(cl);

			a.jmp(SELECT_VALUE_LANG(0x1413AAED7, 0x0));
		}

		const char* get_custom_map_name2(int map_id)
		{
			const auto steam_matchmaking = (*game::SteamMatchmaking)();

			static char buffer[64]{};

			if (map_id >= 8)
			{
				if (did_match_list)
				{
					const auto lobby_id = matchmaking::get_last_requested_lobby();
					const auto name = steam_matchmaking->__vftable->GetLobbyData(steam_matchmaking, lobby_id, "map_name");
					strncpy_s(buffer, sizeof(buffer), name, _TRUNCATE);
				}
				else
				{
					const auto map_name = custom_maps::get_map_name(static_cast<std::uint8_t>(map_id));
					strncpy_s(buffer, sizeof(buffer), map_name.data(), _TRUNCATE);
				}

				return buffer;
			}
			else
			{
				game::fox::StringId id{};
				id.id = builtin_map_names[map_id];
				return game::tpp::ui::utility::GetLangText(id);
			}
		}

		void match_list_map_name_stub6(utils::hook::assembler& a)
		{
			a.mov(rbx, qword_ptr(rsi));

			a.push(rax);
			a.pushad64();
			a.mov(ecx, dword_ptr(r14, 8));
			a.call_aligned(get_custom_map_name2);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);

			a.jmp(SELECT_VALUE_LANG(0x1413A66B1, 0x0));
		}

		utils::hook::detour get_map_count_hook;
		int get_map_count_stub()
		{
			const auto result = get_map_count_hook.invoke<int>();
			if (result != 8) // not the maps
			{
				return result;
			}

			return static_cast<int>(custom_maps::get_map_count());
		}

		void get_map_name_stub(__int64 a1, unsigned int map_id, char* buffer, unsigned int len)
		{
			did_match_list = false;

			if (map_id >= 8)
			{
				const auto map_name = custom_maps::get_map_name(static_cast<std::uint8_t>(map_id));
				strncpy_s(buffer, len, map_name.data(), _TRUNCATE);
			}
			else
			{
				game::fox::StringId id{};
				id.id = builtin_map_names[map_id];
				const auto map_name_str = game::tpp::ui::utility::GetLangText(id);
				strncpy_s(buffer, len, map_name_str, _TRUNCATE);
			}
		}

		void set_rush_text_stub(__int64 a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5)
		{
			const auto vtable = *reinterpret_cast<__int64**>(a1);
			utils::hook::invoke<void>(vtable[232], a1, a2, a3, "MOD", a5);
		}

		void set_rush_value_stub(__int64 a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5)
		{
			const auto vtable = *reinterpret_cast<__int64**>(a1);
			utils::hook::invoke<void>(vtable[232], a1, a2, a3, "MOD", a5);
		}

		const char* get_server_mod(__int64 entry)
		{
			static char buffers[32][32]{};

			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			const auto entry_unk = *reinterpret_cast<__int64*>(entry + 6);
			const auto lobby_info_ptr = entry_unk - 200;
			const auto lobby_index = *reinterpret_cast<int*>(lobby_info_ptr + 40);

			game::steam_id lobby_id{};
			steam_matchmaking->__vftable->GetLobbyByIndex(steam_matchmaking, &lobby_id, lobby_index);

			const auto buffer = buffers[lobby_index % 32];
			const auto name = steam_matchmaking->__vftable->GetLobbyData(steam_matchmaking, lobby_id, "mod_path");
			strncpy_s(buffer, 32, name, _TRUNCATE);

			return buffer;
		}

		void match_list_rush_value_stub(utils::hook::assembler& a)
		{
			a.mov(rcx, qword_ptr(rsp, 0x38));

			a.push(r9);
			a.pushad64();
			a.call_aligned(get_server_mod);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(r9);

			a.mov(rdx, qword_ptr(rsp, 0x70));
			a.mov(rcx, qword_ptr(rsp, 0x30));
			a.mov(rax, qword_ptr(rsp, 0xE0));
			a.mov(r8, qword_ptr(rax, 0x28));

			a.jmp(SELECT_VALUE_LANG(0x1413BA24E, 0x0));
		}

		void update_host_comment()
		{
			matchmaking::set_lobby_data("host_comment_string", var_match_host_comment->current.get_string());
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			if (!game::environment::is_mgo())
			{
				return;
			}

			var_match_host_comment = vars::register_string("match_host_comment", "", vars::var_flag_saved, "custom match host comment (max 256 chars)");

			utils::hook::call(SELECT_VALUE_LANG(0x1413BA09B, 0x0), append_host_comment_stub);

			// handle custom map id
			utils::hook::jump(SELECT_VALUE_LANG(0x1413A622F, 0x0), utils::hook::assemble(match_list_map_name_stub1), true);
			utils::hook::jump(SELECT_VALUE_LANG(0x1413BC199, 0x0), utils::hook::assemble(match_list_map_name_stub2), true);
			utils::hook::jump(SELECT_VALUE_LANG(0x1413BA28B, 0x0), utils::hook::assemble(match_list_map_name_stub3), true);
			utils::hook::jump(SELECT_VALUE_LANG(0x1413BB230, 0x0), utils::hook::assemble(match_list_map_name_stub4), true);
			utils::hook::jump(SELECT_VALUE_LANG(0x1413AAEC9, 0x0), utils::hook::assemble(match_list_map_name_stub5), true, true);
			utils::hook::jump(SELECT_VALUE_LANG(0x1413A669C, 0x0), utils::hook::assemble(match_list_map_name_stub6), true);
			// todo: 01413A676E, rush mode

			// map select
			get_map_count_hook.create(SELECT_VALUE_LANG(0x1413A74B0, 0x0), get_map_count_stub);
			utils::hook::jump(SELECT_VALUE_LANG(0x1413A7620, 0x0), get_map_name_stub);
			//utils::hook::jump(SELECT_VALUE_LANG(0x1413B92A0, 0x0), get_map_name_stub); // map filter

			// replace "RUSH" with "MOD"
			utils::hook::nop(SELECT_VALUE_LANG(0x1413A9545, 0x0), 6);
			utils::hook::call(SELECT_VALUE_LANG(0x1413A9545, 0x0), set_rush_text_stub);
			utils::hook::jump(SELECT_VALUE_LANG(0x1413BA235, 0x0), utils::hook::assemble(match_list_rush_value_stub), true);
		}

		void start() override
		{
			if (!game::environment::is_mgo())
			{
				return;
			}

			var_match_host_comment->set_callback = update_host_comment;
			matchmaking::register_callback(matchmaking::event_create_lobby, [](game::mgo_match_t*, game::steam_id)
			{
				update_host_comment();
			});
		}
	};
}

REGISTER_COMPONENT(match_list::component)
