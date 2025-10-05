#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "input.hpp"
#include "lobby.hpp"
#include "ui.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace text_chat::input
{
	namespace
	{
		void handle_char(chat_state_t& state, char c)
		{
			if (std::strlen(state.input) >= chat_message_max_len)
			{
				return;
			}

			std::memmove(state.input + state.cursor + 1, state.input + state.cursor, chat_message_max_len - state.cursor);
			state.input[state.cursor] = c;
			state.cursor++;

			if (state.cursor == chat_message_max_len)
			{
				state.input[state.cursor] = 0;
			}
		}

		void handle_backspace(chat_state_t& state)
		{
			if (state.cursor <= 0)
			{
				return;
			}

			std::memmove(state.input + state.cursor - 1, state.input + state.cursor,
				std::strlen(state.input) + 1 - state.cursor);
			state.cursor--;
		}

		void handle_tab(chat_state_t& state)
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

		void handle_return(chat_state_t& state)
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
					lobby::send_chat_message(state.input);
				}
			}

			stop_typing(state);
		}

		void handle_paste(chat_state_t& state)
		{
			const auto clipboard = utils::string::get_clipboard_data();

			for (const auto& c : clipboard)
			{
				if (state.cursor >= chat_message_max_len)
				{
					return;
				}

				if (is_char_text(c))
				{
					handle_char(state, c);
				}
			}
		}

		void move_cursor(chat_state_t& state, bool right)
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

		void handle_delete(chat_state_t& state)
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

		void handle_up(chat_state_t& state)
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

		void handle_down(chat_state_t& state)
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

		void update_history(chat_state_t& state)
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
	}

	bool handle_key(const int key, const bool is_down, const bool is_game_console_bind)
	{
		return chat_state.access<bool>([&](chat_state_t& state)
		{
			if (!state.is_typing || (state.mode == mode_console && is_game_console_bind))
			{
				return false;
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
			case 0x16:
				handle_paste(state);
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
			}
			}

			return true;
		});
	}

	bool handle_mousewheel(const bool down)
	{
		return chat_state.access<bool>([&](chat_state_t& state)
		{
			if (!state.is_typing)
			{
				return false;
			}

			const auto max_offset = std::max(0, static_cast<int>(state.messages.size()) - chat_view_size);

			const auto prev_offset = state.chat_offset;
			state.chat_offset = std::min(std::max(0, state.chat_offset + (1 - 2 * down)), max_offset);

			if (state.chat_offset != prev_offset)
			{
				ui::add_sound(chat_scroll_sound_id, 1ms);
			}

			return true;
		});
	}

	void stop_typing(chat_state_t& state)
	{
		state.is_typing = false;
		state.block_input = false;
		state.chat_offset = 0;
		state.mode = mode_none;
		std::memset(state.input, 0, sizeof(state.input));
		state.cursor = 0;
	}

	bool is_input_blocked()
	{
		return chat_state.access<bool>([](chat_state_t& state)
		{
			return state.block_input;
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
			command::add("toggleconsole", []
			{
				if (!is_console_enabled() || !can_show_chat())
				{
					return;
				}

				chat_state.access([](chat_state_t& state)
				{
					if (state.mode == mode_console)
					{
						stop_typing(state);
					}
					else
					{
						stop_typing(state);
						state.is_typing = true;
						state.block_input = true;
						state.mode = mode_console;
					}
				});
			});

			command::add("clearchat", [](const command::params& params)
			{
				reset_chat();
			});

			if (!game::environment::is_mgo())
			{
				return;
			}

			command::add("chatall", []
			{
				if (!is_chat_enabled() || !can_show_chat() || !can_use_chat())
				{
					return;
				}

				chat_state.access([](chat_state_t& state)
				{
					stop_typing(state);
					state.is_typing = true;
					state.block_input = true;
					state.mode = mode_chat;
				});
			});
		}
	};
}

REGISTER_COMPONENT(text_chat::input::component)
