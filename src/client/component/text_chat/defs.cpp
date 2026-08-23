#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "defs.hpp"
#include "ui.hpp"
#include "input.hpp"
#include "../renderer/fonts.hpp"
#include "../matchmaking.hpp"
#include "../session.hpp"

#include <utils/string.hpp>

namespace text_chat
{
	vars::var_ptr var_chat_enable;

	vars::var_ptr var_chat_message_time;
	vars::var_ptr var_chat_input_bg;
	vars::var_ptr var_chat_output_bg;
	vars::var_ptr var_chat_slider_color;
	vars::var_ptr var_chat_input_pulse;
	vars::var_ptr var_chat_scale;
	vars::var_ptr var_chat_offset;
	vars::var_ptr var_chat_height;
	vars::var_ptr var_chat_width;
	vars::var_ptr var_chat_direction;

	bool initialized;

	utils::concurrency::container<chat_state_t, std::recursive_mutex> chat_state;

	const wchar_t* chat_prefixes[mode_count]{};

	bool is_chat_enabled()
	{
		return var_chat_enable->current.enabled();
	}

	bool can_use_chat()
	{
		if (!session::session_info.is_connected)
		{
			return false;
		}

		if (game::environment::is_mgo() && matchmaking::get_match() == nullptr)
		{
			return false;
		}

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

		return true;
	}

	std::size_t clean_message(const std::wstring& msg, wchar_t* out, const std::size_t max_len)
	{
		std::size_t index = 0;
		const auto count = std::min(msg.size(), max_len);

		for (auto i = 0; i < count; i++)
		{
			if (!renderer::is_char_printable(msg[i]))
			{
				continue;
			}

			out[index++] = msg[i];
		}

		return index;
	}

	void clear()
	{
		chat_state.access([](chat_state_t& state)
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
			if (game::environment::is_dedi())
			{
				return;
			}

			var_chat_enable = vars::register_bool("chat_enable", true, vars::var_flag_saved, "enable mgo text chat");

			var_chat_message_time = vars::register_int("chat_time", 10000, 0, 60000, vars::var_flag_saved, "chat message duration");

			vars::color_t bg{};
			bg.a = 0.5f;

			vars::color_t input_bg{};
			input_bg.a = 0.5f;

			vars::color_t slider_color{};
			slider_color.r = 1.f;
			slider_color.g = 1.f;
			slider_color.b = 1.f;
			slider_color.a = 0.6f;

			var_chat_input_bg = vars::register_color("chat_input_bg",
				bg, vars::var_flag_saved, "chat input background color");

			var_chat_output_bg = vars::register_color("chat_output_bg",
				input_bg, vars::var_flag_saved, "chat output background color");

			var_chat_slider_color = vars::register_color("chat_slider_color",
				slider_color, vars::var_flag_saved, "chat slider color");

			var_chat_input_pulse = vars::register_bool("chat_input_pulse", 
				true, vars::var_flag_saved, "enable chat input box outline pulse");

			var_chat_height = vars::register_int("chat_height", 
				6, 1, 10, vars::var_flag_saved, "chat height");

			var_chat_scale = vars::register_float("chat_scale",
				1.f, 0.1f, 2.f, vars::var_flag_saved, "chat scale");
		
			if (game::environment::is_mgo())
			{
				chat_prefixes[mode_none] = L"";
				chat_prefixes[mode_chat] = L"say to all";
				chat_prefixes[mode_chat_team] = L"say to team";

				var_chat_offset = vars::register_vec2("chat_offset",
					{100.f, 600.f}, 0.f, 1500.f, vars::var_flag_saved, "chat offset");

				var_chat_direction = vars::register_int("chat_direction",
					0, 0, 1, vars::var_flag_saved, "chat direction (0: up (default), 1: down)");

				var_chat_width = vars::register_float("chat_width",
					350.f, 0.f, 10000.f, vars::var_flag_saved, "chat width");
			}
			else
			{
				chat_prefixes[mode_none] = L"";
				chat_prefixes[mode_chat] = L"say";
				chat_prefixes[mode_chat_team] = L"say";

				var_chat_offset = vars::register_vec2("chat_offset",
					{900.f, 350.f}, 0.f, 1500.f, vars::var_flag_saved, "chat offset");

				var_chat_direction = vars::register_int("chat_direction",
					0, 0, 1, vars::var_flag_saved, "chat direction (0: up (default), 1: down)");

				var_chat_width = vars::register_float("chat_width",
					300.f, 0.f, 10000.f, vars::var_flag_saved, "chat width");
			}
		}

		void game_initialized() override
		{
			initialized = true;
		}
	};
}

REGISTER_COMPONENT(text_chat::component)
