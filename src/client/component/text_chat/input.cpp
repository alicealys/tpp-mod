#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "text_chat.hpp"
#include "input.hpp"
#include "lobby.hpp"
#include "ui.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace text_chat::input
{
	namespace
	{
		utils::hook::detour wnd_proc_hook;
		utils::hook::detour get_raw_input_for_windows_hook;

		void stop_typing(chat_state_t& state)
		{
			state.is_typing = false;
			state.block_input = false;
			state.chat_offset = 0;
			std::memset(state.input, 0, sizeof(state.input));
			state.cursor = 0;
		}

		void handle_char(chat_state_t& state, char c)
		{
			if (state.cursor < chat_message_max_len)
			{
				state.input[state.cursor++] = c;
			}
		}

		void handle_backspace(chat_state_t& state)
		{
			if (state.cursor <= 0)
			{
				return;
			}

			state.input[--state.cursor] = 0;
		}

		void handle_tab(chat_state_t& state)
		{
			if (state.input[0] != '/')
			{
				return;
			}

			const auto partial_name = &state.input[1];
			const auto command_name_opt = command::find_command_name(partial_name);
			if (!command_name_opt.has_value())
			{
				return;
			}

			const auto& command_name = command_name_opt.value();
			if (command_name.size() >= sizeof(state.input) - 1)
			{
				return;
			}

			std::memcpy(partial_name, command_name.data(), command_name.size());
			state.cursor = static_cast<int>(command_name.size()) + 1;
			state.input[state.cursor++] = ' ';
		}

		void handle_return(chat_state_t& state)
		{
			if (state.input[0] == '/')
			{
				command::execute(&state.input[1]);
			}
			else
			{
				lobby::send_chat_message(state.input);
			}

			stop_typing(state);
		}

		std::optional<char> get_chat_key()
		{
			const auto key = var_chat_key->current.get<std::string>();
			if (key.size() > 0)
			{
				return {static_cast<char>(toupper(key[0]))};
			}

			return {};
		}

		void handle_typing(UINT msg, WPARAM w_param, LPARAM l_param)
		{
			const auto key_opt = get_chat_key();
			if (!is_chat_enabled() || !can_use_chat() || !key_opt.has_value())
			{
				return;
			}

			const auto key = key_opt.value();

			chat_state.access([&](chat_state_t& state)
			{
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
					if (w_param == key)
					{
						state.block_input = true;
					}

					break;
				}
				case WM_CHAR:
				{
					const auto c = static_cast<char>(w_param);
					if (!state.is_typing)
					{
						if (toupper(c) == key)
						{
							state.is_typing = true;
						}

						return;
					}

					if (is_char_text(c))
					{
						handle_char(state, c);
					}
					else
					{
						switch (w_param)
						{
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
						}
					}

					break;
				}
				}
			});
		}

		LRESULT wnd_proc_stub(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
		{
			handle_typing(msg, w_param, l_param);
			return wnd_proc_hook.invoke<LRESULT>(hwnd, msg, w_param, l_param);
		}

		LRESULT get_raw_input_for_windows_stub(WPARAM w_param, LPARAM l_param, HWND hwnd, void* a4)
		{
			const auto is_input_blocked = chat_state.access<bool>([](chat_state_t& state)
			{
				return state.block_input;
			});

			if (is_chat_enabled() && is_input_blocked)
			{
				return 0;
			}

			return get_raw_input_for_windows_hook.invoke<LRESULT>(w_param, l_param, hwnd, a4);
		}
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

			wnd_proc_hook.create(0x140070FC0, wnd_proc_stub);
			get_raw_input_for_windows_hook.create(0x14CB496A0, get_raw_input_for_windows_stub);
		}
	};
}

REGISTER_COMPONENT(text_chat::input::component)
