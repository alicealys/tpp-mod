#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "vars.hpp"
#include "matchmaking.hpp"

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
				utils::hook::invoke<void>(SELECT_VALUE_LANG(0x1413B8B40, 0x0), a1, a2, a3, a4);
			}
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
			var_match_host_comment->set_callback = update_host_comment;

			utils::hook::call(SELECT_VALUE_LANG(0x1413BA11B, 0x0), append_host_comment_stub);
		}

		void start() override
		{
			if (!game::environment::is_mgo())
			{
				return;
			}

			matchmaking::register_callback(matchmaking::event_create_lobby, [](game::mgo_match_t*, game::steam_id)
			{
				update_host_comment();
			});
		}
	};
}

REGISTER_COMPONENT(match_list::component)
