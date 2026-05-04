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

		bool is_device_open()
		{
			const auto inst = game::tpp::ui::menu::UiCommonDataManager_::GetInstance();
			if (inst == nullptr)
			{
				return false;
			}

			return game::tpp::ui::menu::impl::MotherBaseDeviceSystemImpl_::IsDeviceOpend();
		}

		void draw_overlay(game::fox::gr::dg::plugins::Draw2DRenderer* instance)
		{
			const auto fps = static_cast<int>(cg_perf.average);

			const auto main_session = *game::s_pSession;
			const auto rtt = session::get_rtt(main_session);

			const auto fps_color = fps >= 60 ? color_good : (fps >= 30 ? color_ok : color_bad);
			const auto ping_color = rtt < 100 ? color_good : (rtt < 200 ? color_ok : color_bad);

			const auto should_draw_ping = var_ui_draw_ping->current.enabled() && main_session != nullptr;

			auto margin = 8.f;
			auto offset_x = 1280.f - margin;
			auto box_width = 0.f;

			constexpr const auto font_size = 14.f;

			const auto ping_text = utils::string::va("%ims", rtt);
			const auto fps_text = utils::string::va("%i", fps);

			const auto fps_width = renderer::get_text_width("fps: ", font_size) +
				renderer::get_text_width(fps_text, font_size);

			const auto ping_width = renderer::get_text_width("ping: ", font_size) + 
				renderer::get_text_width(ping_text, font_size);

			if (var_ui_draw_fps->current.enabled())
			{
				box_width += fps_width;
			}

			if (should_draw_ping)
			{
				box_width += ping_width;
			}

			if (box_width > 0.f)
			{
				box_width += 3.f;
				renderer::draw_box(instance, offset_x - box_width, margin, box_width, font_size + 4.f, color_bg);
			}

			if (var_ui_draw_fps->current.enabled())
			{
				offset_x -= fps_width;
				renderer::draw_text(instance, "fps:", font_size, offset_x - 1.f, margin + 1.f, color_text, color_outline);
				renderer::draw_text(instance, utils::string::va("%i", fps), font_size, offset_x + 23.f, margin + 1.5f, fps_color, color_outline);
			}

			if (should_draw_ping)
			{
				offset_x -= ping_width;
				renderer::draw_text(instance, "ping:", font_size, offset_x, margin + 1.f, color_text, color_outline);
				renderer::draw_text(instance, ping_text, font_size, offset_x + 30.f, margin + 1.5f, ping_color, color_outline);
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
		}

		void start() override
		{
			renderer::on_frame(draw_overlay);
			scheduler::loop(perf_update, scheduler::pipeline::main);
		}
	};
}

REGISTER_COMPONENT(overlay::component)
