#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "input.hpp"
#include "ui.hpp"
#include "lobby.hpp"
#include "../session.hpp"

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
			std::string input = state.input;
			const auto space_index = input.find_last_of(' ');
			if (space_index != std::string::npos)
			{
				input = input.substr(space_index + 1);
			}

			if (input.empty())
			{
				return;
			}

			auto is_self = false;
			std::string name;
			const auto client = session::get_client_by_name(input, &is_self, &name);
			if (client == nullptr)
			{
				return;
			}

			if (name.size() >= sizeof(state.input) - 1)
			{
				return;
			}

			state.cursor -= static_cast<int>(input.size());

			for (auto c : name)
			{
				if (state.cursor >= chat_message_max_len)
				{
					return;
				}

				c = utils::string::normalize_ascii_extended(c);
				if (utils::string::is_char_text(c))
				{
					state.input[state.cursor++] = c;
				}
			}

			handle_char(state, ' ');
		}

		void handle_return(chat_state_t& state)
		{
			if (state.input[0] != 0)
			{
				text_chat::lobby::send_chat_message(state.input, state.mode == mode_chat_team);
			}

			stop_typing(state);
		}

		void handle_paste(chat_state_t& state)
		{
			const auto clipboard = utils::string::get_clipboard_data();

			for (auto c : clipboard)
			{
				if (state.cursor >= chat_message_max_len)
				{
					return;
				}

				c = utils::string::normalize_ascii_extended(c);
				if (utils::string::is_char_text(c))
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

		void clear_input(chat_state_t& state)
		{
			state.cursor = 0;
			std::memset(state.input, 0, sizeof(state.input));
		}
	}

	bool handle_key(const int key, const bool is_down, const bool is_game_console_bind)
	{
		return chat_state.access<bool>([&](chat_state_t& state)
		{
			if (!state.is_typing)
			{
				return false;
			}

			if (key == VK_CONTROL)
			{
				state.ctrl_down = is_down;
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
			case VK_ESCAPE:
				stop_typing(state);
				break;
			case VK_TAB:
				handle_tab(state);
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
		return chat_state.access<bool>([&](chat_state_t& state)
		{
			if (!state.is_typing)
			{
				return false;
			}

			if (!is_down || key >= 0xFF)
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
				const auto c = utils::string::normalize_ascii_extended(static_cast<char>(key));
				if (utils::string::is_char_text(c))
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
		return chat_state.access<bool>([&](chat_state_t& state)
		{
			if (!state.is_typing)
			{
				return false;
			}

			const auto scroll_amount = state.ctrl_down ? 50.f : 10.f;
			state.view_text_offset_y = std::max(0.f, state.view_text_offset_y + scroll_amount * (down ? -1.f : 1.f));
			return true;
		});
	}

	void stop_typing(chat_state_t& state)
	{
		state.is_typing = false;
		state.mode = mode_none;
		std::memset(state.input, 0, sizeof(state.input));
		state.cursor = 0;
		state.view_text_offset_y = 0.f;
		state.history_index = -1;
	}

	bool is_input_blocked()
	{
		return chat_state.access<bool>([](chat_state_t& state)
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
			if (!game::environment::is_mgo() || game::environment::is_dedi())
			{
				return;
			}

			chat_state.access([](chat_state_t& state)
			{
				state = {};
			});

			command::add("clearchat", [](const command::params& params)
			{
				clear();
			});

			command::add("chatall", []
			{
				if (!is_chat_enabled() || !can_use_chat())
				{
					return;
				}

				chat_state.access([](chat_state_t& state)
				{
					stop_typing(state);
					state.is_typing = true;
					state.mode = mode_chat;
				});
			});

			command::add("chatteam", []
			{
				if (!is_chat_enabled() || !can_use_chat())
				{
					return;
				}

				chat_state.access([](chat_state_t& state)
				{
					stop_typing(state);
					state.is_typing = true;
					state.mode = mode_chat_team;
				});
			});
		}
	};
}

REGISTER_COMPONENT(text_chat::input::component)
