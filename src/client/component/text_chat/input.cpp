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
				std::memset(state.input, 0, sizeof(state.input));
				state.cursor = 0;
				state.mode = mode_console;
			}
			else if (state.mode == mode_chat)
			{
				if (state.input[0] != 0)
				{
					lobby::send_chat_message(state.input);
				}

				stop_typing(state);
			}
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
	}

	void handle_typing(UINT msg, WPARAM w_param, LPARAM l_param)
	{
		chat_state.access([&](chat_state_t& state)
		{
			if (!is_console_enabled() && !is_chat_enabled())
			{
				stop_typing(state);
				return;
			}

			switch (msg)
			{
			case WM_MOUSEWHEEL:
			{
				if (state.is_typing)
				{
					const auto delta = GET_WHEEL_DELTA_WPARAM(w_param);
					const auto decrease = delta < 0;
					const auto max_offset = std::max(0, static_cast<int>(state.messages.size()) - chat_view_size);

					const auto prev_offset = state.chat_offset;
					state.chat_offset = std::min(std::max(0, state.chat_offset + (1 - 2 * decrease)), max_offset);

					if (state.chat_offset != prev_offset)
					{
						ui::add_sound(chat_scroll_sound_id, 1ms);
					}
				}

				break;
			}
			case WM_KEYDOWN:
			{
				switch (w_param)
				{
				case VK_LEFT:
					move_cursor(state, false);
					break;
				case VK_RIGHT:
					move_cursor(state, true);
					break;
				}
				break;
			}
			case WM_CHAR:
			{
				const auto c = static_cast<char>(w_param);

				if (is_char_text(c))
				{
					handle_char(state, c);
				}
				else
				{
					switch (w_param)
					{
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
						handle_return(state);
						break;
					case 0x7F:
						handle_delete(state);
						break;
					}
				}

				break;
			}
			}
		});
	}

	void stop_typing(chat_state_t& state)
	{
		state.is_typing = false;
		state.block_input = false;
		state.chat_offset = 0;
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
			if (!game::environment::is_mgo())
			{
				return;
			}

			command::add("clearchat", [](const command::params& params)
			{
				reset_chat();
			});

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
