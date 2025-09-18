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

		void set_log_text(game::tpp::ui::hud::AnnounceLogViewer* this_, const char* text, int index, float alpha)
		{
			if (!game::tpp::ui::hud::AnnounceLogViewer_::ModelInit(this_, static_cast<char>(index)))
			{
				return;
			}

			auto& log_model = this_->logModels[index];

			game::tpp::ui::utility::SetTextForModelNodeText(log_model.modelNodeText1, &log_model.textUnit, text);
			game::tpp::ui::utility::SetTextForModelNodeText(log_model.modelNodeText2, &log_model.textUnit, text);

			log_model.model->__vftable->SetVisible(log_model.model, true);

			const auto uix_utility = game::fox::uix::impl::GetUixUtilityToFeedQuarkEnvironment();
			uix_utility->__vftable->SetAlpha1(uix_utility, log_model.modelNode, alpha);
			uix_utility->__vftable->SetColorRGB5(uix_utility, log_model.modelNode, 1.f, 1.f, 1.f);
		}

		void clear_log_text(game::tpp::ui::hud::AnnounceLogViewer* this_)
		{
			for (auto i = 0; i < 5; i++)
			{
				set_log_text(this_, "", i, 1.f);
			}
		}

		void set_log_visible(game::tpp::ui::hud::AnnounceLogViewer* this_, bool visible)
		{
			for (auto i = 0; i < 5; i++)
			{
				this_->logModels[i].model->__vftable->SetVisible(this_->logModels[i].model, visible);
			}
		}

		void update_chat_sounds(chat_state_t& state)
		{
			const auto now = std::chrono::high_resolution_clock::now();

			const auto ui_inst = game::tpp::ui::menu::UiCommonDataManager_::GetInstance();
			const auto sound_control = game::tpp::ui::menu::UiCommonDataManager_::GetSoundControl(ui_inst);

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
		}

		void update_chat_input(chat_state_t& state, game::tpp::ui::hud::AnnounceLogViewer* log_viewer)
		{
			static message_buffer_t log_buffer{};
			std::memset(log_buffer, 0, sizeof(log_buffer));

			if (state.is_typing)
			{
				const auto now = std::chrono::high_resolution_clock::now();
				const auto ms_epoch = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
				const auto show_cursor = ((ms_epoch % chat_cursor_interval) > chat_cursor_interval / 2);

				const auto len = std::strlen(state.input);

				static const auto prefix_len = std::strlen(chat_input_prefix);
				static const auto text_len = sizeof(log_buffer) - prefix_len;

				std::memcpy(log_buffer, chat_input_prefix, prefix_len);
				auto postfix = &log_buffer[prefix_len];

				std::memcpy(postfix, state.input, state.cursor);
				std::memcpy(&postfix[state.cursor + 1], &state.input[state.cursor], len - state.cursor);

				postfix[state.cursor] = show_cursor ? chat_cursor_char : ' ';
			}

			set_log_text(log_viewer, log_buffer, chat_message_input_index, 1.f);
		}

		void update_chat_messages(chat_state_t& state, game::tpp::ui::hud::AnnounceLogViewer* log_viewer)
		{
			static message_buffer_t log_buffers[chat_view_size]{};
			std::memset(log_buffers, 0, sizeof(log_buffers));

			const auto now = std::chrono::high_resolution_clock::now();

			if (state.is_typing)
			{
				for (auto& message : state.messages)
				{
					message.time = now;
				}
			}

			for (auto i = 0; i < chat_view_size; i++)
			{
				const auto message_index = state.chat_offset + i;
				if (message_index >= state.messages.size())
				{
					continue;
				}

				auto& message = state.messages[message_index];

				const auto diff = now - message.time;
				const auto ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());
				const auto ms_left = var_chat_time->current.get_int() - ms;

				auto alpha = 1.f;

				if (ms_left <= 0)
				{
					continue;
				}
				else if (ms_left <= chat_message_fade_time)
				{
					alpha = static_cast<float>(ms_left) / static_cast<float>(chat_message_fade_time);
				}

				std::memcpy(log_buffers[i], message.buffer, sizeof(message.buffer));
				set_log_text(log_viewer, log_buffers[i], chat_message_input_index - i - 1, alpha);
			}
		}

		void update_chat(chat_state_t& state)
		{
			const auto inst = game::tpp::ui::hud::CommonDataManager_::GetInstance();
			if (inst == nullptr || inst->announceLogViewer == nullptr)
			{
				return;
			}

			static auto was_chat_enabled = false;
			const auto chat_enabled = is_chat_enabled();

			const auto _0 = gsl::finally([&]
			{
				was_chat_enabled = chat_enabled;
			});

			if (!chat_enabled)
			{
				if (was_chat_enabled)
				{
					clear_log_text(inst->announceLogViewer);
				}

				return;
			}

			if (!can_use_chat())
			{
				set_log_visible(inst->announceLogViewer, false);
				return;
			}

			set_log_visible(inst->announceLogViewer, true);

			update_chat_sounds(state);
			update_chat_input(state, inst->announceLogViewer);
			update_chat_messages(state, inst->announceLogViewer);
		}

		void* update_ui_stub(void* a1)
		{
			chat_state.access([&](chat_state_t& state)
			{
				update_chat(state);
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
		const auto inst = game::tpp::ui::hud::CommonDataManager_::GetInstance();
		game::tpp::ui::hud::CommonDataManager_::AnnounceLogView(inst, text.data(), 0, 0, 1);
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
