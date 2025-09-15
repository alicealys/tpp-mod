#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "console.hpp"
#include "command.hpp"
#include "vars.hpp"
#include "scheduler.hpp"

#include <utils/io.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <windowsx.h>

namespace text_chat
{
	namespace
	{
		utils::hook::detour on_lobby_chat_msg_hook;
		utils::hook::detour wnd_proc_hook;
		utils::hook::detour get_raw_input_for_windows_hook;

		vars::var_ptr var_chat_time;
		vars::var_ptr var_chat_key;
		vars::var_ptr var_chat_enable;

		std::mutex chat_mutex;

		constexpr auto chat_history_size = 20;
		constexpr auto chat_size = 5; // do not change
		constexpr auto chat_view_size = chat_size - 1; // ^
		constexpr auto chat_message_input_index = chat_size - 1; // ^
		constexpr auto chat_message_max_len = 50;
		constexpr auto chat_cursor_interval = 500;
		constexpr auto chat_message_sound_id = 150;
		constexpr auto chat_scroll_sound_id = 152;
		constexpr auto chat_cursor_char = '_';
		constexpr auto chat_key_default = 'Y';
		char chat_input_prefix[] = "say: ";

		using message_buffer_t = char[128];

		struct chat_message_t
		{
			message_buffer_t buffer;
			std::chrono::high_resolution_clock::time_point time;
		};

		struct sound_play_t
		{
			unsigned int id;
			bool started;
			std::chrono::high_resolution_clock::time_point start;
			std::chrono::milliseconds duration;
		};

		struct
		{
			std::deque<chat_message_t> messages;
			int chat_offset;
			int message_index;
			bool is_typing;
			bool block_input;
			message_buffer_t input;
			int cursor;
			bool need_play_sound;
			std::vector<sound_play_t> sounds;
		} chat_state{};


		void add_sound(const unsigned int id, const std::chrono::milliseconds duration)
		{
			sound_play_t sound{};
			sound.id = id;
			sound.duration = duration;
			chat_state.sounds.emplace_back(sound);
		}

		void send_chat_message(const std::string& text)
		{
			const auto unk = *game::s_unk1;
			if (unk == nullptr)
			{
				return;
			}

			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			steam_matchmaking->__vftable->SendLobbyChatMsg(steam_matchmaking, unk->unk1->lobby_id, text.data(), static_cast<int>(text.size()));
		}

		void hud_message(const std::string& text)
		{
			const auto inst = game::tpp::ui::hud::CommonDataManager::GetInstance();
			game::tpp::ui::hud::CommonDataManager::AnnounceLogView(inst, text.data(), 0, 0, 1);
		}

		bool is_chat_enabled()
		{
			return var_chat_enable->current.get<bool>();
		}

		bool can_use_chat()
		{
			const auto inst = game::tpp::ui::hud::CommonDataManager::GetInstance();
			const auto ui_inst = game::tpp::ui::menu::UiCommonDataManager::GetInstance();

			if (inst == nullptr || ui_inst == nullptr)
			{
				return false;
			}

			if (game::tpp::ui::menu::UiCommonDataManager::GetPauseMenuType(ui_inst) != 0 ||
				!game::tpp::ui::hud::CommonDataManager::IsEndLoadingTips(inst))
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

		void update_hud_messages()
		{
			const auto inst = game::tpp::ui::hud::CommonDataManager::GetInstance();
			if (inst == nullptr || inst->announceLogViewer == nullptr)
			{
				return;
			}

			std::lock_guard _0(chat_mutex);

			if (!var_chat_enable->current.get<bool>())
			{
				if (var_chat_enable->changed)
				{
					var_chat_enable->changed = false;
					chat_state = {};
				}
				else
				{
					return;
				}
			}

			static message_buffer_t messages[chat_view_size]{};
			static message_buffer_t input{};

			std::memset(messages, 0, sizeof(messages));
			std::memset(input, 0, sizeof(input));

			const auto now = std::chrono::high_resolution_clock::now();

			if (can_use_chat())
			{
				if (chat_state.is_typing)
				{
					const auto ms_epoch = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
					const auto show_cursor = ((ms_epoch % chat_cursor_interval) > chat_cursor_interval / 2);
					_snprintf_s(&input[0], sizeof(input), sizeof(input), "%s%s%c", chat_input_prefix, chat_state.input, show_cursor ? chat_cursor_char : '\0');
				}

				for (auto& message : chat_state.messages)
				{
					if (chat_state.is_typing)
					{
						message.time = now;
					}
				}

				const auto ui_inst = game::tpp::ui::menu::UiCommonDataManager::GetInstance();
				const auto sound_control = game::tpp::ui::menu::UiCommonDataManager::GetSoundControl(ui_inst);

				for (auto sound = chat_state.sounds.begin(); sound != chat_state.sounds.end(); )
				{
					if (!sound->started)
					{
						sound->start = now;
						sound->started = true;
					}

					game::tpp::ui::utility::StartSound(sound_control, sound->id);
					if (now - sound->start >= sound->duration)
					{
						sound = chat_state.sounds.erase(sound);
					}
					else
					{
						++sound;
					}
				}

				for (auto i = 0ull; i < chat_view_size; i++)
				{
					const auto message_index = chat_state.chat_offset + i;
					if (message_index >= chat_state.messages.size())
					{
						continue;
					}

					auto& message = chat_state.messages[message_index];

					const auto diff = now - message.time;
					const auto ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());

					if (!chat_state.is_typing && ms > var_chat_time->current.get<int>())
					{
						continue;
					}

					std::memcpy(messages[i], message.buffer, sizeof(message.buffer));
				}
			}

			for (auto i = 0; i < chat_view_size; i++)
			{
				game::tpp::ui::hud::CommonDataManager::AnnounceLogViewOnly(inst, messages[i], 0, static_cast<char>(chat_message_input_index - i - 1));
			}

			game::tpp::ui::hud::CommonDataManager::AnnounceLogViewOnly(inst, input, 0, static_cast<char>(chat_message_input_index));
		}

		void push_chat_message(const std::string& msg, bool play_sound)
		{
			if (!is_chat_enabled() || !can_use_chat())
			{
				return;
			}

			std::lock_guard _0(chat_mutex);

			chat_message_t message{};
			strncpy_s(message.buffer, sizeof(message.buffer), msg.data(), _TRUNCATE);
			message.time = std::chrono::high_resolution_clock::now();

			chat_state.messages.push_front(message);
			chat_state.chat_offset = 0;

			if (play_sound)
			{
				add_sound(chat_message_sound_id, 1ms);
			}

			if (chat_state.messages.size() > chat_history_size)
			{
				chat_state.messages.pop_back();
			}
		}

		void process_chat_msg(game::LobbyChatMsg_t* msg)
		{
			if (!is_chat_enabled())
			{
				return;
			}

			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			const auto steam_friends = (*game::SteamFriends)();

			game::steam_id user{};
			char buffer[4096]{};
			int chat_entry_type{};
			const auto size = steam_matchmaking->__vftable->GetLobbyChatEntry(steam_matchmaking, msg->lobby_id, msg->chat_id,
				&user, buffer, sizeof(buffer), &chat_entry_type);

			if (size >= 4 && *reinterpret_cast<int*>(buffer) < 10)
			{
				return;
			}

			const auto name = steam_friends->__vftable->GetFriendPersonaName(steam_friends, user);
			buffer[chat_message_max_len] = 0;
			const auto message = utils::string::va("%s: %s", name, buffer);
			push_chat_message(message, true);

			console::info("[chat] %s\n", message);
		}

		int on_lobby_chat_msg_stub(void* a1, game::LobbyChatMsg_t* msg)
		{
			process_chat_msg(msg);
			return on_lobby_chat_msg_hook.invoke<int>(a1, msg);
		}

		void stop_typing()
		{
			chat_state.is_typing = false;
			chat_state.block_input = false;
			chat_state.chat_offset = 0;
			std::memset(chat_state.input, 0, sizeof(chat_state.input));
			chat_state.cursor = 0;
		}

		void handle_char(char c)
		{
			if (chat_state.cursor < chat_message_max_len)
			{
				chat_state.input[chat_state.cursor++] = c;
			}
		}

		void handle_backspace()
		{
			if (chat_state.cursor <= 0)
			{
				return;
			}

			chat_state.input[--chat_state.cursor] = 0;
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

			std::lock_guard _0(chat_mutex);

			switch (msg)
			{
			case WM_MOUSEWHEEL:
			{
				if (chat_state.is_typing)
				{
					const auto delta = GET_WHEEL_DELTA_WPARAM(w_param);
					const auto decrease = delta < 0;
					const auto max_offset = std::max(0, static_cast<int>(chat_state.messages.size()) - chat_view_size);
					
					const auto prev_offset = chat_state.chat_offset;
					chat_state.chat_offset = std::min(std::max(0, chat_state.chat_offset + (1 - 2 * decrease)), max_offset);

					if (chat_state.chat_offset != prev_offset)
					{
						add_sound(chat_scroll_sound_id, 1ms);
					}
				}

				break;
			}
			case WM_KEYDOWN:
			{
				if (w_param == key)
				{
					chat_state.block_input = true;
				}

				break;
			}
			case WM_CHAR:
			{
				const auto c = static_cast<char>(w_param);
				if (!chat_state.is_typing)
				{
					if (toupper(c) == key)
					{
						chat_state.is_typing = true;
					}

					return;
				}

				if (c >= 32 && c < 127)
				{
					handle_char(c);
				}
				else
				{
					switch (w_param)
					{
					case VK_ESCAPE:
					{
						stop_typing();
						break;
					}
					case VK_BACK:
					{
						handle_backspace();
						break;
					}
					case VK_RETURN:
					{
						send_chat_message(chat_state.input);
						stop_typing();
						break;
					}
					}
				}

				break;
			}
			}
		}

		LRESULT wnd_proc_stub(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
		{
			handle_typing(msg, w_param, l_param);
			return wnd_proc_hook.invoke<LRESULT>(hwnd, msg, w_param, l_param);
		}

		LRESULT get_raw_input_for_windows_stub(WPARAM w_param, LPARAM l_param, HWND hwnd, void* a4)
		{
			if (is_chat_enabled() && chat_state.block_input)
			{
				return 0;
			}

			return get_raw_input_for_windows_hook.invoke<LRESULT>(w_param, l_param, hwnd, a4);
		}

		void* update_ui_stub(void* a1)
		{
			update_hud_messages();
			return utils::hook::invoke<void*>(0x1405EA750, a1);
		}

		utils::hook::detour announce_log_view_hook;

		char announce_log_view_stub(void* a1, const char* msg, char a3, char a4, char a5)
		{
			if (*msg != 0)
			{
				push_chat_message(utils::string::va("[game]: %s", msg), false);
				return 0;
			}

			return announce_log_view_hook.invoke<char>(a1, msg, a3, a4, a5);
		}
	}

	void reset_chat()
	{
		std::lock_guard _0(chat_mutex);
		chat_state = {};
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			var_chat_time = vars::register_int("chat_time", 10000, 0, 60000, vars::var_flag_saved, "chat message duration");
			var_chat_key = vars::register_string("chat_key", utils::string::va("%c", chat_key_default), vars::var_flag_saved, "key bind to open text chat");
			var_chat_enable = vars::register_bool("chat_enable", false, vars::var_flag_saved, "enable mgo text chat");
		}

		void start() override
		{
			if (!game::environment::is_mgo())
			{
				return;
			}

			on_lobby_chat_msg_hook.create(0x1405A3DF0, on_lobby_chat_msg_stub);
			wnd_proc_hook.create(0x140070FC0, wnd_proc_stub);
			get_raw_input_for_windows_hook.create(0x14CB496A0, get_raw_input_for_windows_stub);
			announce_log_view_hook.create(0x1405E7610, announce_log_view_stub);

			utils::hook::call(0x14069E649, update_ui_stub);

			// increase max announce log lines
			// utils::hook::nop(0x1463C8746, 7); // vector count
			// utils::hook::set<std::uint16_t>(0x1463C8746, 0xB841); // ^
			// utils::hook::set<std::uint32_t>(0x1463C8746 + 2, chat_size); // ^
			// utils::hook::set<std::uint32_t>(0x14508D9E0 + 1, chat_size * 0x58 + 8); // mem size
			// utils::hook::set<std::uint8_t>(0x1406C9176 + 2, static_cast<std::uint8_t>(chat_size)); // cmp
			// utils::hook::set<std::uint8_t>(0x1463CBF9F + 2, static_cast<std::uint8_t>(chat_size)); // cmp 2

#ifdef DEBUG
			command::add("hudmessage", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto msg = params.join(1);
				hud_message(msg);
			});

			command::add("chatmessage", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto msg = params.join(1);
				push_chat_message(msg, true);
			});
#endif

			command::add("startsound", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto id_s = params.get(1);
				const auto id = static_cast<unsigned int>(std::atoi(id_s.data()));

				const auto ui_inst = game::tpp::ui::menu::UiCommonDataManager::GetInstance();
				if (ui_inst == nullptr)
				{
					return;
				}

				const auto sound_control = game::tpp::ui::menu::UiCommonDataManager::GetSoundControl(ui_inst);
				if (sound_control == nullptr)
				{
					return;
				}

				game::tpp::ui::utility::StartSound(sound_control, id);
			});

			command::add("stopsound", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto id_s = params.get(1);
				const auto id = static_cast<unsigned int>(std::atoi(id_s.data()));

				const auto ui_inst = game::tpp::ui::menu::UiCommonDataManager::GetInstance();
				if (ui_inst == nullptr)
				{
					return;
				}

				const auto sound_control = game::tpp::ui::menu::UiCommonDataManager::GetSoundControl(ui_inst);
				if (sound_control == nullptr)
				{
					return;
				}

				game::tpp::ui::utility::StopSound(sound_control, id);
			});

			command::add("clearchat", [](const command::params& params)
			{
				std::lock_guard _0(chat_mutex);
				chat_state = {};
			});

			command::add("say", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto msg = params.join(1);
				send_chat_message(msg);
			});
		}
	};
}

REGISTER_COMPONENT(text_chat::component)
