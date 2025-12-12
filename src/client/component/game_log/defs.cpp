#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "defs.hpp"
#include "ui.hpp"

#include <utils/string.hpp>

namespace game_log
{
	vars::var_ptr var_chat_enable;
	vars::var_ptr var_console_enable;

	vars::var_ptr var_game_log_message_time;
	vars::var_ptr var_game_log_input_width;
	vars::var_ptr var_game_log_input_bg;
	vars::var_ptr var_game_log_output_bg;
	vars::var_ptr var_game_log_scale;
	vars::var_ptr var_game_log_offset;
	vars::var_ptr var_game_log_line_spacing;
	vars::var_ptr var_game_log_height;
	vars::var_ptr var_game_log_width;
	vars::var_ptr var_game_log_max_message_len;

	char chat_input_prefix[] = "say: ";
	char console_input_prefix[] = "> ";

	bool initialized;

	utils::concurrency::container<game_log_state_t, std::recursive_mutex> game_log_state;

	bool is_chat_enabled()
	{
		if (!game::environment::is_mgo())
		{
			return false;
		}

		return var_chat_enable->current.enabled();
	}

	bool is_console_enabled()
	{
		return var_console_enable->current.enabled();
	}

	bool can_show_game_log()
	{
		const auto inst = game::tpp::ui::hud::CommonDataManager_::GetInstance();
		const auto ui_inst = game::tpp::ui::menu::UiCommonDataManager_::GetInstance();

		if (inst == nullptr || ui_inst == nullptr)
		{
			return false;
		}

		if (game::environment::is_tpp())
		{
			if (inst->tpp.announceLogViewer == nullptr)
			{
				return false;
			}
		}
		else
		{
			if (inst->mgo.announceLogViewer == nullptr)
			{
				return false;
			}
		}

		if (game::tpp::ui::menu::UiCommonDataManager_::GetPauseMenuType(ui_inst) != 0 || 
			!game::tpp::ui::hud::CommonDataManager_::IsEndLoadingTips(inst))
		{
			return false;
		}

		return true;
	}

	bool can_use_chat()
	{
		if (!game::environment::is_mgo())
		{
			return false;
		}

		const auto match_container = game::s_mgoMatchMakingManager->match_container;
		const auto session = *game::s_pSession;
		if (match_container == nullptr || session == nullptr || match_container->match->lobby_id.bits == 0 || 
			session->sessionInterface.__vftable->IsConnecting(&session->sessionInterface))
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

	void reset_log()
	{
		game_log_state.access([](game_log_state_t& state)
		{
			state = {};
		});
	}

	bool is_initialized()
	{
		return initialized;
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			var_chat_enable = vars::register_bool("chat_enable", true, vars::var_flag_saved, "enable mgo text chat");

			var_console_enable = vars::register_bool("game_console_enable", false, vars::var_flag_saved, "enable game console");

			var_game_log_message_time = vars::register_int("game_log_message_time", 10000, 0, 60000, vars::var_flag_saved, "game log message duration");

			var_game_log_input_width = vars::register_float("game_log_input_width",
				50.f, 0.f, 1000.f, vars::var_flag_saved, "game log input background width");

			vars::color_t input_bg{};
			input_bg.r = 0.0;
			input_bg.g = 0.0;
			input_bg.b = 0.0;
			input_bg.a = 0.5;

			var_game_log_input_bg = vars::register_color("game_log_input_bg",
				input_bg, vars::var_flag_saved, "game log input background color");

			var_game_log_output_bg = vars::register_color("game_log_output_bg",
				input_bg, vars::var_flag_saved, "game log output background color");

			var_game_log_scale = vars::register_float("game_log_scale",
				1.f, 0.f, 1000.f, vars::var_flag_saved, "game log scale");

			var_game_log_offset = vars::register_vec2("game_log_offset",
				{}, vars::var_flag_saved, "game log offset");

			var_game_log_line_spacing = vars::register_float("game_log_line_spacing",
				2.f, 0.f, 1000.f, vars::var_flag_saved, "game log line spacing");

			var_game_log_height = vars::register_int("game_log_height", 
				4, 1, game_log_view_size, vars::var_flag_saved, "game log height");

			var_game_log_width = vars::register_float("game_log_width", 
				100.f, 0.f, 10000.f, vars::var_flag_saved, "game log width");

			var_game_log_max_message_len = vars::register_int("game_log_message_max_len", 128, 0, 
				sizeof(message_buffer_t) - 1, vars::var_flag_saved, "game log maximum message length");
		}

		void start() override
		{
			scheduler::once([]
			{
				initialized = true;
			}, scheduler::main);
		}
	};
}

REGISTER_COMPONENT(game_log::component)
