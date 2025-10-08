#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "input.hpp"
#include "ui.hpp"
#include "../text_chat/lobby.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace game_log::input
{
	namespace
	{
		void handle_char(game_log_state_t& state, char c)
		{
			const auto max_len = var_game_log_max_message_len->current.get_int();
			if (std::strlen(state.input) >= max_len)
			{
				return;
			}

			std::memmove(state.input + state.cursor + 1, state.input + state.cursor, max_len - state.cursor);
			state.input[state.cursor] = c;
			state.cursor++;

			if (state.cursor == max_len)
			{
				state.input[state.cursor] = 0;
			}
		}

		void handle_backspace(game_log_state_t& state)
		{
			if (state.cursor <= 0)
			{
				return;
			}

			std::memmove(state.input + state.cursor - 1, state.input + state.cursor,
				std::strlen(state.input) + 1 - state.cursor);
			state.cursor--;
		}

		void handle_tab(game_log_state_t& state)
		{
			if (state.mode != mode_console)
			{
				return;
			}

			auto name_opt = command::find_command_name(state.input);
			if (!name_opt.has_value())
			{
				name_opt = vars::find_name(state.input);
				if (!name_opt.has_value())
				{
					return;
				}
			}

			const auto& name = name_opt.value();
			if (name.size() >= sizeof(state.input) - 1)
			{
				return;
			}

			std::memcpy(state.input, name.data(), name.size());
			state.cursor = static_cast<int>(name.size());
			state.input[state.cursor++] = ' ';
		}

		void handle_return(game_log_state_t& state)
		{
			if (state.mode == mode_console)
			{
				console::info("]%s", state.input);
				command::execute(state.input);
			}
			else if (state.mode == mode_chat)
			{
				if (state.input[0] != 0)
				{
					text_chat::lobby::send_chat_message(state.input);
				}
			}

			stop_typing(state);
		}

		void handle_paste(game_log_state_t& state)
		{
			const auto clipboard = utils::string::get_clipboard_data();
			const auto max_len = var_game_log_max_message_len->current.get_int();

			for (const auto& c : clipboard)
			{
				if (state.cursor >= max_len)
				{
					return;
				}

				if (is_char_text(c))
				{
					handle_char(state, c);
				}
			}
		}

		void move_cursor(game_log_state_t& state, bool right)
		{
			if (right && state.input[state.cursor] != '\0')
			{
				state.cursor++;
			}
			else if (!right && state.cursor > 0)
			{
				state.cursor--;
			}
		}

		void handle_delete(game_log_state_t& state)
		{
			auto is_first = true;
			while (state.cursor > 0)
			{
				const auto is_letter = isalnum(state.input[state.cursor - 1]);

				if (is_letter || is_first)
				{
					handle_backspace(state);
				}

				if (!is_letter)
				{
					return;
				}

				is_first = false;
			}
		}

		void handle_up(game_log_state_t& state)
		{
			if (++state.history_index >= state.history.size())
			{
				state.history_index = static_cast<int>(state.history.size()) - 1;
			}

			std::memset(state.input, 0, sizeof(state.input));
			state.cursor = 0;

			if (state.history_index != -1)
			{
				strncpy_s(state.input, state.history.at(state.history_index).data(), sizeof(state.input));
				state.cursor = static_cast<int>(std::strlen(state.input));
			}
		}

		void handle_down(game_log_state_t& state)
		{
			if (--state.history_index < -1)
			{
				state.history_index = -1;
			}

			std::memset(state.input, 0, sizeof(state.input));
			state.cursor = 0;

			if (state.history_index != -1)
			{
				strncpy_s(state.input, state.history.at(state.history_index).data(), sizeof(state.input));
				state.cursor = static_cast<int>(strlen(state.input));
			}
		}

		void update_history(game_log_state_t& state)
		{
			if (state.history_index != -1)
			{
				const auto itr = state.history.begin() + state.history_index;

				if (*itr == state.input)
				{
					state.history.erase(state.history.begin() + state.history_index);
				}
			}

			if (state.input[0])
			{
				state.history.push_front(state.input);
			}

			if (state.history.size() > 10)
			{
				state.history.erase(state.history.begin() + 10);
			}

			state.history_index = -1;
		}

		void clear_input(game_log_state_t& state)
		{
			state.cursor = 0;
			std::memset(state.input, 0, sizeof(state.input));
		}
	}

	bool handle_key(const int key, const bool is_down, const bool is_game_console_bind)
	{
		return game_log_state.access<bool>([&](game_log_state_t& state)
		{
			if (!state.is_typing || (state.mode == mode_console && is_game_console_bind))
			{
				return false;
			}

			if (key == VK_CONTROL)
			{
				state.is_ctrl_down = is_down;
			}

			if (!is_down)
			{
				return true;
			}

			switch (key)
			{
			case VK_UP:
				handle_up(state);
				break;
			case VK_DOWN:
				handle_down(state);
				break;
			case VK_LEFT:
				move_cursor(state, false);
				break;
			case VK_RIGHT:
				move_cursor(state, true);
				break;
			case VK_TAB:
				handle_tab(state);
				break;
			case VK_ESCAPE:
				stop_typing(state);
				break;
			case VK_BACK:
				handle_backspace(state);
				break;
			case VK_RETURN:
				update_history(state);
				handle_return(state);
				break;
			}

			return true;
		});
	}

	bool handle_char(const int key, const bool is_down)
	{
		return game_log_state.access<bool>([&](game_log_state_t& state)
		{
			if (!state.is_typing)
			{
				return false;
			}

			if (!is_down)
			{
				return true;
			}

			switch (key)
			{
			case 0x1:
				clear_input(state);
				break;
			case 0x16:
				handle_paste(state);
				break;
			case 0x7F:
				handle_delete(state);
				break;
			default:
			{
				const auto c = static_cast<char>(key);
				if (is_char_text(c))
				{
					handle_char(state, c);
				}
				break;
			}
			}

			return true;
		});
	}

	bool handle_mousewheel(const bool down)
	{
		return game_log_state.access<bool>([&](game_log_state_t& state)
		{
			if (!state.is_typing)
			{
				return false;
			}

			if (state.is_ctrl_down)
			{
				state.view_text_offset_x = std::max(0.f, state.view_text_offset_x + 30.f * (down ? -1.f : 1.f));
			}
			else
			{
				const auto max_offset = std::max(0, static_cast<int>(state.messages.size()) -
					var_game_log_height->current.get_int());

				const auto prev_offset = state.chat_offset;
				state.chat_offset = std::min(std::max(0, state.chat_offset + (1 - 2 * down)), max_offset);

				if (state.chat_offset != prev_offset)
				{
					ui::add_sound(game_log_scroll_sound_id, 1ms);
				}
			}

			return true;
		});
	}

	void stop_typing(game_log_state_t& state)
	{
		state.is_typing = false;
		state.chat_offset = 0;
		state.mode = mode_none;
		std::memset(state.input, 0, sizeof(state.input));
		state.cursor = 0;
		state.view_text_offset_x = 0.f;
		state.history_index = -1;
	}

	bool is_input_blocked()
	{
		return game_log_state.access<bool>([](game_log_state_t& state)
		{
			return state.is_typing;
		});
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{

		}

		void start() override
		{
			game_log_state.access([](game_log_state_t& state)
			{
				state = {};
			});

			command::add("toggleconsole", []
			{
				if (!is_console_enabled() || !can_show_game_log())
				{
					return;
				}

				game_log_state.access([](game_log_state_t& state)
				{
					if (state.mode == mode_console)
					{
						stop_typing(state);
					}
					else
					{
						stop_typing(state);
						state.is_typing = true;
						state.mode = mode_console;
					}
				});
			});

			command::add("clearlog", [](const command::params& params)
			{
				reset_log();
			});

			if (!game::environment::is_mgo())
			{
				return;
			}

			command::add("chatall", []
			{
				if (!is_chat_enabled() || !can_show_game_log() || !can_use_chat())
				{
					return;
				}

				game_log_state.access([](game_log_state_t& state)
				{
					stop_typing(state);
					state.is_typing = true;
					state.mode = mode_chat;
				});
			});
		}
	};
}

REGISTER_COMPONENT(game_log::input::component)
