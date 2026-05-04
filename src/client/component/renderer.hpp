#pragma once

namespace renderer
{
	float get_text_width(const char* text, float height);

	float draw_text(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height,
		float x, float y, float* color, float* outline_color = nullptr, bool formatted = false);

	float draw_text_with_cursor(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, int cursor,
		float height, float x, float y, float* color, float* outline_color = nullptr, bool formatted = false);

	void draw_box(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float width, 
		float height, float* color, float* outline_color = nullptr, float outline_thickness = 1.f);

	void on_frame(const std::function<void(game::fox::gr::dg::plugins::Draw2DRenderer*)>& cb);
}
