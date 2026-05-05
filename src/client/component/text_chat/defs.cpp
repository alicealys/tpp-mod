#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "defs.hpp"
#include "ui.hpp"
#include "input.hpp"

#include <utils/string.hpp>

namespace text_chat
{
	vars::var_ptr var_chat_enable;

	vars::var_ptr var_chat_message_time;
	vars::var_ptr var_chat_input_bg;
	vars::var_ptr var_chat_output_bg;
	vars::var_ptr var_chat_scale;
	vars::var_ptr var_chat_offset;
	vars::var_ptr var_chat_height;
	vars::var_ptr var_chat_width;

	char chat_input_prefix[] = "say: ";

	bool initialized;

	utils::concurrency::container<chat_state_t, std::recursive_mutex> chat_state;

	bool is_chat_enabled()
	{
		if (!game::environment::is_mgo())
		{
			return false;
		}

		return var_chat_enable->current.enabled();
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

	std::string clean_message(const std::string& msg)
	{
		std::string clean;
		clean.resize(msg.size());
		
		auto index = 0;
		for (auto i = 0; i < msg.size(); i++)
		{
			auto c = utils::string::normalize_ascii_extended(msg[i]);
			if (!utils::string::is_char_text(c))
			{
				continue;
			}

			clean[index++] = c;
		}

		return clean;
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
			var_chat_enable = vars::register_bool("chat_enable", true, vars::var_flag_saved, "enable mgo text chat");

			var_chat_message_time = vars::register_int("chat_time", 10000, 0, 60000, vars::var_flag_saved, "chat message duration");

			vars::color_t bg{};
			bg.a = 0.5f;

			vars::color_t input_bg{};
			input_bg.a = 0.5f;

			var_chat_input_bg = vars::register_color("chat_input_bg",
				bg, vars::var_flag_saved, "chat input background color");

			var_chat_output_bg = vars::register_color("chat_output_bg",
				input_bg, vars::var_flag_saved, "chat output background color");

			var_chat_offset = vars::register_vec2("chat_offset",
				{100.f, 500.f}, vars::var_flag_saved, "chat offset");

			var_chat_height = vars::register_int("chat_height", 
				6, 1, 10, vars::var_flag_saved, "chat height");

			var_chat_width = vars::register_float("chat_width", 
				350.f, 0.f, 10000.f, vars::var_flag_saved, "chat width");
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

REGISTER_COMPONENT(text_chat::component)
