#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "defs.hpp"
#include "ui.hpp"
#include "input.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace game_log::ui
{
	namespace
	{
		utils::hook::detour announce_log_view_hook;

		void reset_font_metrics(game::fox::ui::ModelNodeText* node)
		{
			if (node->packetBuffer == nullptr)
			{
				return;
			}

			auto ptr = reinterpret_cast<size_t>(node->packetBuffer->packet);
			auto offset = 216;
			auto packet = reinterpret_cast<game::fox::gr::Packet2D*>(ptr + offset);

			/*
			auto offset = 0;
			while (packet->type != 8 && offset < node->packetBuffer->packetSize && packet->type != 0)
			{
				offset += packet->size;
				packet = reinterpret_cast<game::fox::gr::Packet2D*>(ptr + offset);
			}*/

			if (packet->command == 8)
			{
				const auto packet_string = reinterpret_cast<game::fox::gr::Packet2DString*>(packet);
				packet_string->fontMetricsCache = nullptr;
			}
		}

		template <typename VtableType>
		float set_log_text_internal(game::tpp::ui::hud::AnnounceLogViewer* this_, const char* text, int index, 
			int y_offset_count, float alpha, bool auto_scroll)
		{
			const auto model_index = index / 2;

			if (!game::tpp::ui::hud::AnnounceLogViewer_::ModelInit(this_, static_cast<char>(model_index)))
			{
				return 0.f;
			}

			auto& log_model = this_->logModels[model_index];

			const auto model_node_text_index = index % 2;
			const auto model_node_text = model_node_text_index == 0 ? log_model.modelNodeText1 : log_model.modelNodeText2;

			model_node_text->flags |= 2;

			const auto width = var_game_log_width->current.get_float();
			
			model_node_text->displayAreaWidth = (width - 0.5f) / (model_node_text->displaySizes.values[0]);
			model_node_text->displayAreaHeight = 1.f;

			reset_font_metrics(model_node_text);

			game::tpp::ui::utility::SetTextForModelNodeText(model_node_text, &log_model.textUnit, text);
			
			const auto get_text_width = [&]
			{
				float text_width{};
				game::fox::gr::Font_::GetStringWidth(text, &text_width, false, model_node_text->f4);
				return text_width * 20.f;
			};

			const auto display_width = (model_node_text->displaySizes.values[0] * 10.f) * model_node_text->displayAreaWidth;
			const auto text_width = get_text_width();

			const auto uix_utility = game::fox::uix::impl::GetUixUtilityToFeedQuarkEnvironment();
			auto vtable = reinterpret_cast<VtableType*>(&uix_utility->__vftable->tpp);

			const auto offset = var_game_log_offset->current.get_vec2();
			const auto scale = var_game_log_scale->current.get_float();
			const auto line_spacing = var_game_log_line_spacing->current.get_float();

			const auto is_text_overflowing = (text_width > display_width + 50.f);

			model_node_text->textOffsetX = 0.f;
			model_node_text->textAlign = 0;
			vtable->SetTranslationX1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(model_node_text), 0.f);

			if (auto_scroll && is_text_overflowing)
			{
				model_node_text->textAlign = 2;
				model_node_text->textOffsetX = -1.f * display_width;
				vtable->SetTranslationX1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(model_node_text), -width);
			}

			log_model.model->__vftable->SetVisible(log_model.model, true);

			const auto y_offset = model_index * 3.f - 15.f;
			const auto y_offset_text = (index + y_offset_count) * line_spacing;

			vtable->SetScale1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(log_model.modelNode), scale);

			vtable->SetTranslationX1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(log_model.modelNode), offset.x);
			vtable->SetTranslationY1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(log_model.modelNode), offset.y + y_offset);

			vtable->SetTranslationY1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(model_node_text), y_offset_text);

			vtable->SetColorRGB5(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(log_model.modelNodeText1), 1.f, 1.f, 1.f);
			vtable->SetColorRGB5(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(log_model.modelNodeText2), 1.f, 1.f, 1.f);

			vtable->SetAlpha1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(model_node_text), alpha);
			vtable->SetAlpha1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(log_model.modelNode), 1.f);

			return text_width;
		}

		float set_log_text(game::tpp::ui::hud::AnnounceLogViewer* this_, const char* text, int index, int y_offset_count = 0, float alpha = 0.f,
			bool auto_scroll = false)
		{
			if (game::environment::is_tpp())
			{
				return set_log_text_internal<game::fox::uix::impl::UixUtilityImpl_vtbl_tpp>(
					this_, text, index, y_offset_count, alpha, auto_scroll
				);
			}
			else
			{
				return set_log_text_internal<game::fox::uix::impl::UixUtilityImpl_vtbl_mgo>(
					this_, text, index, y_offset_count, alpha, auto_scroll
				);
			}
		}

		template <typename VtableType>
		float update_log_text_offset_internal(game::tpp::ui::hud::AnnounceLogViewer* this_, float max_text_width, float text_offset_x)
		{
			const auto get_display_width = [&]
			{
				auto& log_model = this_->logModels[0];
				return (log_model.modelNodeText1->displaySizes.values[0] * 10.f) * log_model.modelNodeText1->displayAreaWidth;
			};

			const auto display_width = get_display_width();
			const auto margin = display_width + 50.f;

			if (max_text_width > margin)
			{
				text_offset_x = std::min(text_offset_x, max_text_width - display_width);
			}
			else
			{
				text_offset_x = 0.f;
			}

			for (auto i = 0; i < game_log_view_size; i++)
			{
				const auto index = game_log_view_index_begin + i;
				const auto model_index = index / 2;
				auto& log_model = this_->logModels[model_index];

				const auto model_node_text_index = index % 2;
				const auto model_node_text = model_node_text_index == 0 ? log_model.modelNodeText1 : log_model.modelNodeText2;

				if (max_text_width > margin)
				{
					model_node_text->textOffsetX = text_offset_x;
				}
				else
				{
					model_node_text->textOffsetX = 0.f;
				}
			}

			return text_offset_x;
		}

		float update_log_text_offset(game::tpp::ui::hud::AnnounceLogViewer* this_, float max_text_width, float text_offset_x)
		{
			if (game::environment::is_tpp())
			{
				return update_log_text_offset_internal<game::fox::uix::impl::UixUtilityImpl_vtbl_tpp>(
					this_, max_text_width, text_offset_x
				);
			}
			else
			{
				return update_log_text_offset_internal<game::fox::uix::impl::UixUtilityImpl_vtbl_mgo>(
					this_, max_text_width, text_offset_x
				);
			}
		}

		template <typename VtableType>
		void set_log_background_internal(game::tpp::ui::hud::AnnounceLogViewer* this_,
			const int model_index, float width, float height, float y_offset, const float* bg_color)
		{
			if (!game::tpp::ui::hud::AnnounceLogViewer_::ModelInit(this_, static_cast<char>(model_index)))
			{
				return;
			}

			auto& log_model = this_->logModels[model_index];

			const auto uix_utility = game::fox::uix::impl::GetUixUtilityToFeedQuarkEnvironment();
			auto vtable = reinterpret_cast<VtableType*>(&uix_utility->__vftable->tpp);

			game::Vectormath::Aos::Vector3 vector1_1{};

			game::Vectormath::Aos::Vector3 vector2_1{};
			game::Vectormath::Aos::Vector3 vector2_2{};

			vector1_1.values[0] = 0.f;
			vector1_1.values[1] = -1.f;

			vector2_1.values[0] = 1.f;
			vector2_1.values[1] = height;

			vector2_2.values[0] = width;
			vector2_2.values[1] = height;

			vtable->SetVertexTranslate(uix_utility, log_model.modelNodeMesh, 0xF0992C92, &vector1_1, &vector2_1);
			vtable->SetVertexTranslate(uix_utility, log_model.modelNodeMesh, 0x761CA0B9, &vector1_1, &vector2_2);
			vtable->SetVertexTranslate(uix_utility, log_model.modelNodeMesh, 0x7F3D9C6A, &vector1_1, &vector2_1);
			vtable->SetVertexTranslate(uix_utility, log_model.modelNodeMesh, 0x27571000, &vector1_1, &vector2_2);

			vtable->SetTranslationY1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(log_model.modelNodeMesh), y_offset);

			vtable->SetColorRGB5(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(log_model.modelNodeMesh),
				bg_color[0], bg_color[1], bg_color[2]);

			vtable->SetAlpha1(uix_utility, reinterpret_cast<game::fox::ui::ModelNode*>(log_model.modelNodeMesh), bg_color[3]);
		}

		void set_log_output_background(game::tpp::ui::hud::AnnounceLogViewer* this_, const float* bg_color, int lines)
		{
			const auto width = var_game_log_width->current.get_float() * 1.f;
			const auto height = lines * 1.f * (var_game_log_line_spacing->current.get_float() / 2.f) + 0.2f;

			if (game::environment::is_tpp())
			{
				set_log_background_internal<game::fox::uix::impl::UixUtilityImpl_vtbl_tpp>(this_, 1, width, height, 1.4f, bg_color);
			}
			else
			{
				set_log_background_internal<game::fox::uix::impl::UixUtilityImpl_vtbl_mgo>(this_, 1, width, height, 1.4f, bg_color);
			}
		}

		void set_log_input_background(game::tpp::ui::hud::AnnounceLogViewer* this_, const float* bg_color)
		{
			const auto width = var_game_log_width->current.get_float() * 1.f;
			const auto height = 1.2f;

			if (game::environment::is_tpp())
			{
				set_log_background_internal<game::fox::uix::impl::UixUtilityImpl_vtbl_tpp>(this_, 0, width, height, -1.2f, bg_color);
			}
			else
			{
				set_log_background_internal<game::fox::uix::impl::UixUtilityImpl_vtbl_mgo>(this_, 0, width, height, -1.2f, bg_color);
			}
		}

		void clear_log_text(game::tpp::ui::hud::AnnounceLogViewer* this_)
		{
			for (auto i = 0; i < 10; i++)
			{
				set_log_text(this_, "", i);
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

		void update_chat_sounds(game_log_state_t& state)
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

		void update_chat_input(game_log_state_t& state, game::tpp::ui::hud::AnnounceLogViewer* log_viewer)
		{
			static message_buffer_t log_buffer{};
			std::memset(log_buffer, 0, sizeof(log_buffer));

			if (state.is_typing)
			{
				const auto now = std::chrono::high_resolution_clock::now();
				const auto ms_epoch = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
				const auto show_cursor = ((ms_epoch % game_log_cursor_interval) > game_log_cursor_interval / 2);

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

				postfix[state.cursor] = show_cursor ? game_log_cursor_char : ' ';
			}

			set_log_text(log_viewer, log_buffer, game_log_message_input_index, 0, 1.f, true);
		}

		int update_chat_messages(game_log_state_t& state, game::tpp::ui::hud::AnnounceLogViewer* log_viewer)
		{
			static message_buffer_t log_buffers[game_log_view_size]{};
			std::memset(log_buffers, 0, sizeof(log_buffers));

			const auto now = std::chrono::high_resolution_clock::now();

			if (state.is_typing)
			{
				for (auto& message : state.messages)
				{
					message.time = now;
				}
			}

			auto active_messages = 0;
			auto max_text_width = 0.f;

			for (auto i = 0; i < game_log_view_size; i++)
			{
				const auto message_index = state.chat_offset + i;

				auto alpha = 0.f;

				if (message_index < state.messages.size() && i < var_game_log_height->current.get_int())
				{
					const auto update_buffer = [&]
					{
						auto& message = state.messages[message_index];

						const auto diff = now - message.time;
						const auto ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());
						const auto ms_left = var_game_log_message_time->current.get_int() - ms;

						alpha = 1.f;

						if (ms_left <= 0)
						{
							alpha = 0.f;
							return;
						}
						else if (ms_left <= game_log_message_fade_time)
						{
							alpha = static_cast<float>(ms_left) / static_cast<float>(game_log_message_fade_time);
						}

						++active_messages;

						std::memcpy(log_buffers[i], message.buffer, sizeof(message.buffer));
					};

					update_buffer();
				}

				const auto width = set_log_text(log_viewer, log_buffers[i], game_log_view_index_begin + i, -1, alpha);
				max_text_width = std::max(max_text_width, width);
			}

			state.view_text_offset_x = update_log_text_offset(log_viewer, max_text_width, state.view_text_offset_x);

			return active_messages;
		}

		void update_game_log_background(game_log_state_t& state, game::tpp::ui::hud::AnnounceLogViewer* log_viewer, int message_count)
		{
			const auto input_color = var_game_log_input_bg->current.get_color();
			const auto output_color = var_game_log_output_bg->current.get_color();
			const auto height = var_game_log_height->current.get_int();

			float empty_color[4]{};
			set_log_input_background(log_viewer, empty_color);
			set_log_output_background(log_viewer, empty_color, 0);

			if (state.is_typing)
			{
				set_log_input_background(log_viewer, &input_color.r);
				set_log_output_background(log_viewer, &output_color.r, height);
			}
			else if (game::tpp::ui::menu::impl::MotherBaseDeviceSystemImpl_::IsDeviceOpend() && message_count > 0)
			{
				set_log_input_background(log_viewer, empty_color);
				set_log_output_background(log_viewer, &output_color.r, message_count);
			}
		}

		void update_chat_internal(game_log_state_t& state, game::tpp::ui::hud::AnnounceLogViewer* log_viewer)
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

			if (!can_show_game_log())
			{
				input::stop_typing(state);
				set_log_visible(log_viewer, false);
				return;
			}

			set_log_visible(log_viewer, true);

			update_chat_sounds(state);
			update_chat_input(state, log_viewer);
			const auto msg_count = update_chat_messages(state, log_viewer);
			update_game_log_background(state, log_viewer, msg_count);
		}

		void update_chat()
		{
			game_log_state.access([&](game_log_state_t& state)
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
		if (!is_initialized() || !is_chat_enabled() && !is_console_enabled())
		{
			return;
		}

		if (!check_message(msg))
		{
			return;
		}

		game_log_state.access([&](game_log_state_t& state)
		{
			game_log_message_t message{};
			strncpy_s(message.buffer, sizeof(message.buffer), msg.data(), _TRUNCATE);	
			message.time = std::chrono::high_resolution_clock::now();

			const auto max_len = var_game_log_max_message_len->current.get_int();
			message.buffer[max_len] = 0;

			state.messages.push_front(message);
			state.chat_offset = 0;

			if (play_sound)
			{
				add_sound(game_log_message_sound_id, 0ms);
			}

			if (state.messages.size() > game_log_history_size)
			{
				state.messages.pop_back();
			}
		});
	}

	void add_sound(const unsigned int id, const std::chrono::milliseconds duration)
	{
		game_log_state.access([&](game_log_state_t& state)
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
		}
	};
}

REGISTER_COMPONENT(game_log::ui::component)
