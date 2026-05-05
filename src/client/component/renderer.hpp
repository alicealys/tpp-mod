#pragma once

namespace renderer
{
	float calc_text_width(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height);

	float draw_text(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height,
		float x, float y, float* color, float* outline_color = nullptr, bool formatted = false, 
		float display_width = 0.f, float display_height = 0.f, float scroll_x = 0.f, float scroll_y = 0.f, bool word_breaks = false);

	float draw_text_with_cursor(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, int cursor,
		float height, float x, float y, float* color, float* outline_color = nullptr, bool formatted = false,
		float display_width = 0.f);

	void draw_box(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float width, 
		float height, float* color, float* outline_color = nullptr, float outline_thickness = 1.f);

	void add_stencil(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float width,
		float height);

	void remove_stencil(game::fox::gr::dg::plugins::Draw2DRenderer* instance);

	void on_frame(const std::function<void(game::fox::gr::dg::plugins::Draw2DRenderer*)>& cb);
}
