#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "text_chat.hpp"
#include "ui.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace text_chat::ui
{
	namespace
	{
		utils::hook::detour announce_log_view_hook;

		void update_hud_messages(chat_state_t& state)
		{
			const auto inst = game::tpp::ui::hud::CommonDataManager::GetInstance();
			if (inst == nullptr || inst->announceLogViewer == nullptr)
			{
				return;
			}

			if (!var_chat_enable->current.get<bool>())
			{
				if (var_chat_enable->changed)
				{
					var_chat_enable->changed = false;
					state = {};
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
				if (state.is_typing)
				{
					const auto ms_epoch = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
					const auto show_cursor = ((ms_epoch % chat_cursor_interval) > chat_cursor_interval / 2);
					_snprintf_s(&input[0], sizeof(input), sizeof(input), "%s%s%c", chat_input_prefix, state.input, show_cursor ? chat_cursor_char : '\0');
				}

				for (auto& message : state.messages)
				{
					if (state.is_typing)
					{
						message.time = now;
					}
				}

				const auto ui_inst = game::tpp::ui::menu::UiCommonDataManager::GetInstance();
				const auto sound_control = game::tpp::ui::menu::UiCommonDataManager::GetSoundControl(ui_inst);

				for (auto sound = state.sounds.begin(); sound != state.sounds.end(); )
				{
					if (!sound->started)
					{
						sound->start = now;
						sound->started = true;
					}

					game::tpp::ui::utility::StartSound(sound_control, sound->id);
					if (now - sound->start >= sound->duration)
					{
						sound = state.sounds.erase(sound);
					}
					else
					{
						++sound;
					}
				}

				for (auto i = 0ull; i < chat_view_size; i++)
				{
					const auto message_index = state.chat_offset + i;
					if (message_index >= state.messages.size())
					{
						continue;
					}

					auto& message = state.messages[message_index];

					const auto diff = now - message.time;
					const auto ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());

					if (!state.is_typing && ms > var_chat_time->current.get<int>())
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

		void* update_ui_stub(void* a1)
		{
			chat_state.access([&](chat_state_t& state)
			{
				update_hud_messages(state);
			});

			return utils::hook::invoke<void*>(0x1405EA750, a1);
		}

		char announce_log_view_stub(void* a1, const char* msg, char a3, char a4, char a5)
		{
			if (is_chat_enabled() && *msg != 0)
			{
				ui::print(msg, false);
				return 0;
			}

			return announce_log_view_hook.invoke<char>(a1, msg, a3, a4, a5);
		}
	}

	void hud_message(const std::string& text)
	{
		const auto inst = game::tpp::ui::hud::CommonDataManager::GetInstance();
		game::tpp::ui::hud::CommonDataManager::AnnounceLogView(inst, text.data(), 0, 0, 1);
	}

	void print(const std::string& msg, bool play_sound)
	{
		console::info("%s\n", msg.data());

		if (!is_chat_enabled())
		{
			hud_message(msg);
			return;
		}

		if (!can_use_chat() || !check_message(msg))
		{
			return;
		}

		chat_state.access([&](chat_state_t& state)
		{
			chat_message_t message{};
			strncpy_s(message.buffer, sizeof(message.buffer), msg.data(), _TRUNCATE);
			message.time = std::chrono::high_resolution_clock::now();

			state.messages.push_front(message);
			state.chat_offset = 0;

			if (play_sound)
			{
				add_sound(chat_message_sound_id, 1ms);
			}

			if (state.messages.size() > chat_history_size)
			{
				state.messages.pop_back();
			}
		});
	}

	void add_sound(const unsigned int id, const std::chrono::milliseconds duration)
	{
		chat_state.access([&](chat_state_t& state)
		{
			sound_play_t sound{};
			sound.id = id;
			sound.duration = duration;
			state.sounds.emplace_back(sound);
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

			announce_log_view_hook.create(0x1405E7610, announce_log_view_stub);

			utils::hook::call(0x14069E649, update_ui_stub);

#ifdef DEBUG
			command::add("hudmessage", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto msg = params.join(1);
				ui::hud_message(msg);
			});

			command::add("chatmessage", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto msg = params.join(1);
				ui::print(msg, true);
			});
#endif
		}
	};
}
	
REGISTER_COMPONENT(text_chat::ui::component)
