#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "text_chat.hpp"
#include "ui.hpp"
#include "input.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace text_chat::ui
{
	namespace
	{
		utils::hook::detour announce_log_view_hook;

		bool initialized = false;

		void set_log_text(game::tpp::ui::hud::AnnounceLogViewer* this_, const char* text, int index, float alpha, float min_bg_width = 0.f)
		{
			if (!game::tpp::ui::hud::AnnounceLogViewer_::ModelInit(this_, static_cast<char>(index)))
			{
				return;
			}

			auto& log_model = this_->logModels[index];

			const auto reset_font_metrics = [](game::fox::ui::ModelNodeText* node)
			{
				if (node->packetBuffer == nullptr)
				{
					return;
				}

				const auto ptr = reinterpret_cast<size_t>(node->packetBuffer->packet) + 100;
				const auto packet_string = reinterpret_cast<game::fox::gr::Packet2DString*>(ptr);
				packet_string->fontMetricsCache = nullptr;
			};

			reset_font_metrics(log_model.modelNodeText1);
			reset_font_metrics(log_model.modelNodeText2);

			game::tpp::ui::utility::SetTextForModelNodeText(log_model.modelNodeText1, &log_model.textUnit, text);
			game::tpp::ui::utility::SetTextForModelNodeText(log_model.modelNodeText2, &log_model.textUnit, text);

			log_model.model->__vftable->SetVisible(log_model.model, true);

			const auto uix_utility = game::fox::uix::impl::GetUixUtilityToFeedQuarkEnvironment();

			game::fox::StringId string1{};
			game::fox::StringId string2{};

			string1.id = 0x1F66761CA0B9;
			string2.id = 0x43FD27571000;

			game::Vectormath::Aos::Vector3 vector1{};
			game::Vectormath::Aos::Vector3 vector2{};

			vector1.values[0] = log_model.textUnit.textWidth * 0.1f + 0.5f;
			vector1.values[1] = 0.f;
			vector1.values[2] = 0.f;
			vector1.values[3] = 0.f;

			vector2.values[0] = 0.f;
			vector2.values[1] = 1.f;
			vector2.values[2] = 1.f;
			vector2.values[3] = 0.f;

			vector1.values[0] = std::max(min_bg_width, vector1.values[0]);

			const auto show_bg = min_bg_width > 0.f || *text != '\0' && game::tpp::ui::menu::impl::MotherBaseDeviceSystemImpl_::IsDeviceOpend();
			const auto bg_alpha = show_bg ? alpha * 0.85f : 0.f;

			if (game::environment::is_tpp())
			{
				uix_utility->__vftable->tpp.SetAlpha1(uix_utility, log_model.modelNode, alpha);
				uix_utility->__vftable->tpp.SetColorRGB5(uix_utility, log_model.modelNode, 1.f, 1.f, 1.f);

				uix_utility->__vftable->tpp.SetAlpha1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(log_model.modelNodeMesh), bg_alpha);

				uix_utility->__vftable->tpp.SetVertexTranslate(uix_utility, log_model.modelNodeMesh, string1, &vector1, &vector2);
				uix_utility->__vftable->tpp.SetVertexTranslate(uix_utility, log_model.modelNodeMesh, string2, &vector1, &vector2);
				uix_utility->__vftable->tpp.SetColorRGB5(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(log_model.modelNodeMesh), 0.f, 0.f, 0.f);
			}
			else
			{
				uix_utility->__vftable->mgo.SetAlpha1(uix_utility, log_model.modelNode, alpha);
				uix_utility->__vftable->mgo.SetColorRGB5(uix_utility, log_model.modelNode, 1.f, 1.f, 1.f);

				uix_utility->__vftable->mgo.SetAlpha1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(log_model.modelNodeMesh), bg_alpha);

				uix_utility->__vftable->mgo.SetVertexTranslate(uix_utility, log_model.modelNodeMesh, string1, &vector1, &vector2);
				uix_utility->__vftable->mgo.SetVertexTranslate(uix_utility, log_model.modelNodeMesh, string2, &vector1, &vector2);
				uix_utility->__vftable->mgo.SetColorRGB5(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(log_model.modelNodeMesh), 0.f, 0.f, 0.f);
			}
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
			const auto uix_utility = game::fox::uix::impl::GetUixUtilityToFeedQuarkEnvironment();
			for (auto i = 0; i < 5; i++)
			{
				this_->logModels[i].model->__vftable->SetVisible(this_->logModels[i].model, visible);
				if (game::environment::is_tpp())
				{
					uix_utility->__vftable->tpp.SetVisible1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(this_->logModels[i].modelNodeMesh), visible);
				}
				else
				{
					uix_utility->__vftable->mgo.SetVisible1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(this_->logModels[i].modelNodeMesh), visible);
				}
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

			set_log_text(log_viewer, log_buffer, chat_message_input_index, 1.f);

			if (!state.is_typing)
			{
				return;
			}

			const auto now = std::chrono::high_resolution_clock::now();
			const auto ms_epoch = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
			const auto show_cursor = ((ms_epoch % chat_cursor_interval) > chat_cursor_interval / 2);

			const auto len = std::strlen(state.input);

			char* postfix{};

			switch (state.mode)
			{
			case mode_chat:
			{
				static const auto prefix_len = std::strlen(chat_input_prefix);
				static const auto text_len = sizeof(log_buffer) - prefix_len;

				std::memcpy(log_buffer, chat_input_prefix, prefix_len);
				postfix = &log_buffer[prefix_len];
				break;
			}
			case mode_console:
			{
				static const auto prefix_len = std::strlen(console_input_prefix);
				static const auto text_len = sizeof(log_buffer) - prefix_len;

				std::memcpy(log_buffer, console_input_prefix, prefix_len);
				postfix = &log_buffer[prefix_len];
				break;
			}
			default:
				return;
			}

			std::memcpy(postfix, state.input, state.cursor);
			std::memcpy(&postfix[state.cursor + 1], &state.input[state.cursor], len - state.cursor);

			postfix[state.cursor] = show_cursor ? chat_cursor_char : ' ';

			set_log_text(log_viewer, log_buffer, chat_message_input_index, 1.f, 50.f);
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

				auto alpha = 1.f;

				if (message_index < state.messages.size())
				{
					auto& message = state.messages[message_index];

					const auto diff = now - message.time;
					const auto ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());
					const auto ms_left = var_chat_time->current.get_int() - ms;

					if (ms_left <= 0)
					{
						continue;
					}
					else if (ms_left <= chat_message_fade_time)
					{
						alpha = static_cast<float>(ms_left) / static_cast<float>(chat_message_fade_time);
					}

					std::memcpy(log_buffers[i], message.buffer, sizeof(message.buffer));
				}

				set_log_text(log_viewer, log_buffers[i], chat_message_input_index - i - 1, alpha);
			}
		}

		void update_chat_internal(chat_state_t& state, game::tpp::ui::hud::AnnounceLogViewer* log_viewer)
		{
			if (log_viewer == nullptr)
			{
				return;
			}

			static auto was_chat_enabled = false;
			const auto chat_enabled = is_chat_enabled() || is_console_enabled();

			const auto _0 = gsl::finally([&]
			{
				was_chat_enabled = chat_enabled;
			});

			if (!chat_enabled)
			{
				if (was_chat_enabled)
				{
					clear_log_text(log_viewer);
				}

				return;
			}

			if (!can_show_chat())
			{
				input::stop_typing(state);
				set_log_visible(log_viewer, false);
				return;
			}

			set_log_visible(log_viewer, true);

			update_chat_sounds(state);
			update_chat_input(state, log_viewer);
			update_chat_messages(state, log_viewer);
		}

		void update_chat_post_internal(game::tpp::ui::hud::AnnounceLogViewer* log_viewer)
		{
			if (log_viewer == nullptr)
			{
				return;
			}

			for (auto i = 0; i < 5; i++)
			{
				log_viewer->logModels[i].modelNodeText1->__vftable->ReleasePacketBuffer(log_viewer->logModels[i].modelNodeText1);
				log_viewer->logModels[i].modelNodeText2->__vftable->ReleasePacketBuffer(log_viewer->logModels[i].modelNodeText2);

				game::fox::ui::ModelNodeText_::ReleaseBufferAndCache(log_viewer->logModels[i].modelNodeText1);
				game::fox::ui::ModelNodeText_::ReleaseBufferAndCache(log_viewer->logModels[i].modelNodeText2);
			}
		}

		void update_chat()
		{
			chat_state.access([&](chat_state_t& state)
			{
				const auto inst = game::tpp::ui::hud::CommonDataManager_::GetInstance();
				if (inst == nullptr)
				{
					return;
				}

				if (game::environment::is_tpp())
				{
					update_chat_internal(state, inst->tpp.announceLogViewer);
				}
				else
				{
					update_chat_internal(state, inst->mgo.announceLogViewer);
				}
			});
		}

		char announce_log_view_stub(void* a1, const char* msg, char a3, unsigned __int8 a4, char a5)
		{
			if ((is_chat_enabled() || is_console_enabled()) && *msg != 0)
			{
				ui::print(msg, false);
				return 0;
			}

			return announce_log_view_hook.invoke<char>(a1, msg, a3, a4, a5);
		}
	}

	void print(const std::string& msg, bool play_sound)
	{
		if (!initialized || !is_chat_enabled() && !is_console_enabled())
		{
			return;
		}

		if (!check_message(msg))
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
			announce_log_view_hook.create(SELECT_VALUE(0x140863050, 0x1405E7610), announce_log_view_stub);

			scheduler::loop(update_chat, scheduler::main);

			scheduler::once([]
			{
				initialized = true;
			}, scheduler::main);

		}
	};
}

REGISTER_COMPONENT(text_chat::ui::component)
