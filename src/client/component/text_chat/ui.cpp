#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "defs.hpp"
#include "ui.hpp"
#include "input.hpp"
#include "../renderer.hpp"
#include "../session.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace text_chat::ui
{
	namespace
	{
		utils::hook::detour announce_log_view_hook;

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

		constexpr const auto margin = 4.f;
		constexpr const auto font_height = 15.f;
		constexpr const auto line_height = 17.f;

		float outline_color[4] = {0.f, 0.f, 0.f, 0.7f};

		void draw_scrollbar(game::fox::gr::dg::plugins::Draw2DRenderer* r, chat_state_t& state, int lines, float width, 
			float x, float y, float scroll_height, float scroll_amount, float max_scroll)
		{
			if (scroll_height <= lines * line_height)
			{
				return;
			}

			constexpr const auto scrollbar_width = 4.f;

			float color[4]{};
			color[0] = 1.f;
			color[1] = 1.f;
			color[2] = 1.f;
			color[3] = 0.6f;

			const auto initial_height = (lines * line_height) - margin * 2.f;
			const auto percent = initial_height / scroll_height;
			const auto height = initial_height * percent;
			const auto offset = (initial_height - height) * scroll_amount / max_scroll;
			const auto y_ = y - margin * 2.f - height - offset;

			renderer::draw_box(r, x + width - scrollbar_width - margin, y_, scrollbar_width, height , color);
		}

		void draw_messages(game::fox::gr::dg::plugins::Draw2DRenderer* r, chat_state_t& state, int lines, float width, float x, float y)
		{
			const auto now = std::chrono::high_resolution_clock::now();

			const auto bg_color_var = var_chat_output_bg->current.get_color();
			float bg_color[4]{};
			bg_color[0] = bg_color_var.r;
			bg_color[1] = bg_color_var.g;
			bg_color[2] = bg_color_var.b;
			bg_color[3] = bg_color_var.a;

			auto messages_height = 0.f;
			const auto stencil_size = lines * line_height - margin * 2.f;

			if (state.is_typing)
			{
				for (auto& message : state.messages)
				{
					message.time = now;

					auto text_width = renderer::calc_text_width(r, message.buffer, font_height, true);
					const auto display_width = width - margin * 2.f - 6.f - font_height;

					messages_height += static_cast<int>(std::ceil(text_width / display_width)) * font_height + margin;
				}

				const auto max_scroll = messages_height - stencil_size;
				if (messages_height > stencil_size)
				{
					state.view_text_offset_y = std::min(state.view_text_offset_y, max_scroll);
				}
				else
				{
					state.view_text_offset_y = 0.f;
				}

				if (messages_height > 0.f)
				{
					draw_scrollbar(r, state, lines, width, x, y, messages_height, state.view_text_offset_y, max_scroll);
				}
			}
			else
			{
				state.view_text_offset_y = 0.f;
			}

			if ((state.is_typing || game::tpp::ui::menu::impl::MotherBaseDeviceSystemImpl_::IsDeviceOpend()) && messages_height > 0.f)
			{
				const auto box_height = std::min(messages_height, lines * line_height);
				renderer::draw_box(r, x, y - box_height - margin - 1.f, width, box_height + 1.f, bg_color, bg_color, 1.f);
			}

			renderer::remove_stencil(r);
			renderer::add_stencil(r, x, y - stencil_size - margin * 2.f, width, stencil_size);

			auto y_offset = y - margin;

			for (const auto& message : state.messages)
			{
				const auto diff = now - message.time;
				const auto ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());
				const auto ms_left = var_chat_message_time->current.get_int() - ms;

				auto alpha = 1.f;

				if (ms_left <= 0)
				{
					alpha = 0.f;
					break;
				}
				else if (ms_left <= chat_message_fade_time)
				{
					alpha = static_cast<float>(ms_left) / static_cast<float>(chat_message_fade_time);
				}

				float color[4]{};
				color[0] = 1.f;
				color[1] = 1.f;
				color[2] = 1.f;
				color[3] = alpha;

				float color_outline[4]{};
				color_outline[0] = 0.f;
				color_outline[1] = 0.f;
				color_outline[2] = 0.f;
				color_outline[3] = alpha;

				const auto display_width = width - margin * 2.f - 6.f - font_height;
				auto text_width = renderer::calc_text_width(r, message.buffer, font_height, true);

				y_offset -= static_cast<int>(std::ceil(text_width / display_width)) * font_height + margin;

				renderer::draw_text(r, message.buffer, font_height, x + margin, y_offset + state.view_text_offset_y,
					color, color_outline, true, display_width, 0.f, 0.f, 0.f, true);
			}
			
			renderer::remove_stencil(r);
		}

		void draw_input_text(game::fox::gr::dg::plugins::Draw2DRenderer* r, chat_state_t& state, float width, float x, float y)
		{
			float color[4]{};
			color[0] = 1.f;
			color[1] = 1.f;
			color[2] = 1.f;
			color[3] = 1.f;

			float color_hint[4]{};
			color_hint[0] = color[0];
			color_hint[1] = color[1];
			color_hint[2] = color[2];
			color_hint[3] = 0.5f;

			float color_outline[4]{};
			color_outline[0] = 0.f;
			color_outline[1] = 0.f;
			color_outline[2] = 0.f;
			color_outline[3] = 1.f;

			const auto bg_color_var = var_chat_input_bg->current.get_color();
			float bg_color[4]{};
			bg_color[0] = bg_color_var.r;
			bg_color[1] = bg_color_var.g;
			bg_color[2] = bg_color_var.b;
			bg_color[3] = bg_color_var.a;

			const auto now = std::chrono::high_resolution_clock::now();
			const auto ms_epoch = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
			const auto pulse_alpha = 1.f - (static_cast<float>(ms_epoch % 1500) / 1500.f) * 0.7f;

			float box_outline_color[4]{};
			box_outline_color[0] = 1.f;
			box_outline_color[1] = 1.f;
			box_outline_color[2] = 1.f;
			box_outline_color[3] = pulse_alpha;

			renderer::draw_box(r, x, y, width, line_height, bg_color, box_outline_color, 1.f);

			if (state.input[0] == 0)
			{
				renderer::draw_text(r, "say to all", font_height, x + margin, y, color_hint, outline_color);
			}

			renderer::draw_text_with_cursor(r, state.input, state.cursor, font_height,
				x + margin, y, color, color_outline, true, width - margin * 2.f);
		}

		void draw_chat(game::fox::gr::dg::plugins::Draw2DRenderer* r)
		{
			if (!is_chat_enabled() || !can_use_chat())
			{
				return;
			}

			chat_state.access([&](chat_state_t& state)
			{
				const auto pos = var_chat_offset->current.get_vec2();
				const auto width = var_chat_width->current.get_float();

				update_chat_sounds(state);
				draw_messages(r, state, var_chat_height->current.get_int(), width, pos.x, pos.y);

				if (state.is_typing)
				{
					draw_input_text(r, state, width, pos.x, pos.y);
				}
			});
		}

		char announce_log_view_stub(void* a1, const char* msg, char a3, unsigned __int8 a4, char a5)
		{
			if (is_chat_enabled() && *msg != 0)
			{
				ui::print(msg, false);
				return 0;
			}

			return announce_log_view_hook.invoke<char>(a1, msg, a3, a4, a5);
		}
	}

	void print(const std::string& msg, bool play_sound)
	{
		if (!is_initialized() || !is_chat_enabled())
		{
			return;
		}

		const auto cleaned_msg = clean_message(msg);

		chat_state.access([&](chat_state_t& state)
		{
			chat_message_t message{};
			strncpy_s(message.buffer, sizeof(message.buffer), cleaned_msg.data(), _TRUNCATE);
			message.time = std::chrono::high_resolution_clock::now();

			message.buffer[chat_message_max_len] = 0;

			state.messages.push_front(message);

			if (play_sound)
			{
				add_sound(chat_message_sound_id, 0ms);
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
			renderer::on_frame(draw_chat);
		}

		void start() override
		{
			announce_log_view_hook.create(SELECT_VALUE(0x140863050, 0x1405E7610, 0x140862CE0, 0x1405E7020), announce_log_view_stub);
		}
	};
}

REGISTER_COMPONENT(text_chat::ui::component)
