#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "text_chat.hpp"
#include "ui.hpp"
#include "lobby.hpp"
#include "mutes.hpp"

#include <utils/string.hpp>

namespace text_chat
{
	vars::var_ptr var_chat_time;
	vars::var_ptr var_chat_key;
	vars::var_ptr var_chat_enable;

	char chat_input_prefix[] = "say: ";

	utils::concurrency::container<chat_state_t, std::recursive_mutex> chat_state;

	bool is_chat_enabled()
	{
		return var_chat_enable->current.get<bool>();
	}

	bool can_use_chat()
	{
		const auto inst = game::tpp::ui::hud::CommonDataManager_::GetInstance();
		const auto ui_inst = game::tpp::ui::menu::UiCommonDataManager_::GetInstance();

		if (inst == nullptr || ui_inst == nullptr)
		{
			return false;
		}

		if (game::tpp::ui::menu::UiCommonDataManager_::GetPauseMenuType(ui_inst) != 0 ||
			!game::tpp::ui::hud::CommonDataManager_::IsEndLoadingTips(inst))
		{
			return false;
		}

		const auto unk = *game::s_unk1;
		const auto session = *game::s_pSession;
		if (unk == nullptr || session == nullptr || unk->unk1->lobby_id.bits == 0 || session->sessionInterface.__vftable->IsConnecting(&session->sessionInterface))
		{
			return false;
		}

		return true;
	}

	bool is_char_text(char c)
	{
		return c >= 32 && c <= 126;
	}

	bool check_message(const std::string& msg)
	{
		const auto res = std::ranges::find_if_not(msg.begin(), msg.end(), is_char_text);
		return res == msg.end();
	}

	void reset_chat()
	{
		chat_state.access([](chat_state_t& state)
		{
			state = {};
		});
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			var_chat_time = vars::register_int("chat_time", 10000, 0, 60000, vars::var_flag_saved, "chat message duration");
			var_chat_key = vars::register_string("chat_key", utils::string::va("%c", chat_key_default), vars::var_flag_saved, "key bind to open text chat");
			var_chat_enable = vars::register_bool("chat_enable", true, vars::var_flag_saved, "enable mgo text chat");
		}

		void start() override
		{
			if (!game::environment::is_mgo())
			{
				return;
			}

			// increase max announce log lines
			// utils::hook::nop(0x1463C8746, 7); // vector count
			// utils::hook::set<std::uint16_t>(0x1463C8746, 0xB841); // ^
			// utils::hook::set<std::uint32_t>(0x1463C8746 + 2, chat_size); // ^
			// utils::hook::set<std::uint32_t>(0x14508D9E0 + 1, chat_size * 0x58 + 8); // mem size
			// utils::hook::set<std::uint8_t>(0x1406C9176 + 2, static_cast<std::uint8_t>(chat_size)); // cmp
			// utils::hook::set<std::uint8_t>(0x1463CBF9F + 2, static_cast<std::uint8_t>(chat_size)); // cmp 2

			command::add("clearchat", [](const command::params& params)
			{
				reset_chat();
			});
		}
	};
}

REGISTER_COMPONENT(text_chat::component)
