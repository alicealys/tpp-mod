#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "renderer.hpp"
#include "scheduler.hpp"
#include "session.hpp"
#include "vars.hpp"

#include <utils/hook.hpp>
#include <utils/memory.hpp>
#include <utils/string.hpp>

namespace overlay
{
	namespace
	{
		vars::var_ptr var_ui_draw_fps;
		vars::var_ptr var_ui_draw_ping;

		float color_good[4] = {0.6f, 1.0f, 0.0f, 1.0f};
		float color_ok[4] = {1.0f, 0.7f, 0.3f, 1.0f};
		float color_bad[4] = {1.0f, 0.3f, 0.3f, 1.0f};
		float color_outline[4] = {0.f, 0.f, 0.f, 1.f};
		float color_text[4] = {1.f, 1.f, 1.f, 1.f};
		float color_bg[4] = {0.f, 0.f, 0.f, 0.5f};

		struct cg_perf_data
		{
			std::int32_t history[32];
			std::int32_t count;
			std::int32_t index;
			std::int32_t total;
			float average;
			std::int32_t min;
			std::int32_t max;
		};

		cg_perf_data cg_perf{};

		void perf_calc_fps(cg_perf_data* data, const std::int32_t value)
		{
			data->history[data->index % 32] = value;
			data->min = 0x7FFFFFFF;
			data->max = 0;
			data->average = 0.0f;
			data->total = 0;

			for (auto i = 0; i < data->count; ++i)
			{
				const std::int32_t idx = (data->index - i) % 32;

				if (idx < 0)
				{
					break;
				}

				data->total += data->history[idx];

				if (data->min > data->history[idx])
				{
					data->min = data->history[idx];
				}

				if (data->max < data->history[idx])
				{
					data->max = data->history[idx];
				}
			}

			data->average = static_cast<float>(data->total) / static_cast<float>(data->count);
			++data->index;
		}

		void perf_update()
		{
			cg_perf.count = 32;

			const auto time_system = game::fox::GetTimeSystem();

			perf_calc_fps(&cg_perf, static_cast<int>(1.f / time_system.frameTime));
		}

		game::fox::nt::Member* get_peer_member(game::fox::nt::impl::SessionImpl2* session)
		{
			for (auto i = 1u; i < session->allMembers.size; i++)
			{
				if (session->allMembers.members[i] != nullptr && session->allMembers.members[i]->flags != 0)
				{
					return session->allMembers.members[i];
				}
			}
			
			return nullptr;
		}

		const char* get_ping_text(float** color)
		{
			static char buffer[0x40]{};
			const auto main_session = *game::s_pSession;
			if (main_session == nullptr)
			{
				return nullptr;
			}

			const auto session_state = session::get_state(main_session);
			const auto rtt = session::get_rtt(main_session);

			const auto calc_color = [&](const int rtt)
			{
				*color = rtt < 100 ? color_good : (rtt < 200 ? color_ok : color_bad);
			};

			switch (session_state)
			{
			case 2:
			case 3:
			{
				*color = color_good;
				const auto peer = get_peer_member(main_session);
				if (game::environment::is_tpp() && peer != nullptr && peer->sppSocket != nullptr && peer->sppSocket->tpp.rtt_time != -1)
				{
					game::steam_id user_id{};
					user_id.bits = peer->sessionUserId->userId;
					const auto steam_friends = (*game::SteamFriends)();
					const auto name = steam_friends->__vftable->GetFriendPersonaName(steam_friends, user_id);
					calc_color(peer->sppSocket->tpp.rtt_time);
					snprintf(buffer, sizeof(buffer), "%s - %ims", name, peer->sppSocket->tpp.rtt_time);
					return buffer;
				}
				return "HOST";
			}
			case 4:
			case 5:
			case 6:
				*color = color_ok;
				return "CONNECTING";
			case 7:
			{
				calc_color(rtt);
				snprintf(buffer, sizeof(buffer), "%ims", rtt);
				return buffer;
			}
			}

			return nullptr;
		}

		void draw_overlay(game::fox::gr::dg::plugins::Draw2DRenderer* instance)
		{
			const auto fps = static_cast<int>(cg_perf.average);

			const auto fps_color = fps >= 60 ? color_good : (fps >= 30 ? color_ok : color_bad);

			auto margin = 8.f;
			auto offset_x = 1280.f - margin;
			auto box_width = 0.f;

			constexpr const auto font_size = 14.f;
			const auto line_height = 18.f;

			const auto draw_fps = var_ui_draw_fps->current.enabled();
			const auto draw_ping = var_ui_draw_ping->current.enabled();

			if (!draw_ping && !draw_fps)
			{
				return;
			}

			float* ping_color{};
			const auto ping_text = draw_ping
				? get_ping_text(&ping_color)
				: nullptr;

			const auto fps_text = utils::string::va("%i", fps);

			const auto fps_value_width = renderer::calc_text_width(instance, fps_text, font_size);
			const auto fps_label_width = renderer::calc_text_width(instance, "fps: ", font_size);
			const auto fps_width = fps_label_width + fps_value_width;

			const auto ping_value_width = renderer::calc_text_width(instance, ping_text, font_size);
			const auto ping_label_width = renderer::calc_text_width(instance, "ping: ", font_size);
			const auto ping_width = ping_label_width + ping_value_width;

			if (ping_width == 0.f && fps_width == 0.f)
			{
				return;
			}

			if (draw_fps)
			{
				box_width += fps_width;
			}

			if (ping_text != nullptr)
			{
				if (draw_fps)
				{
					box_width += margin;
				}

				box_width += ping_width;
			}

			if (box_width > 0.f)
			{
				box_width += margin * 2.f;
				renderer::draw_box(instance, offset_x - box_width, margin, box_width, line_height, color_bg);
			}

			offset_x -= margin;
			const auto text_y = margin + 1.5f;

			if (draw_fps)
			{
				offset_x -= fps_width;
				renderer::draw_text(instance, "fps:", font_size, offset_x, text_y, color_text, color_outline);
				renderer::draw_text(instance, utils::string::va("%i", fps), font_size, offset_x + fps_label_width, text_y, fps_color, color_outline);
			}

			if (ping_text != nullptr)
			{
				offset_x -= ping_width;
				if (draw_fps)
				{
					offset_x -= margin;
				}
				renderer::draw_text(instance, "ping:", font_size, offset_x, text_y, color_text, color_outline);
				renderer::draw_text(instance, ping_text, font_size, offset_x + ping_label_width, text_y, ping_color, color_outline);
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			var_ui_draw_fps = vars::register_bool("ui_draw_fps", 0, vars::var_flag_saved, "draw fps counter");
			var_ui_draw_ping = vars::register_bool("ui_draw_ping", 0, vars::var_flag_saved, "draw ping counter");

			renderer::on_frame(draw_overlay);
		}

		void start() override
		{
			scheduler::loop(perf_update, scheduler::pipeline::main);
		}
	};
}

REGISTER_COMPONENT(overlay::component)
