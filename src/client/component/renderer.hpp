#pragma once

namespace renderer
{
	struct params_t
	{
		float rotation;
		float perspective[5];
	};

	float calc_text_width_artist(const char* text, float height, bool formatted = false,
		bool word_wrapping = false, float line_width = 0.f, int* line_count = nullptr, int caret_index = -1, int max_len = -1);

	float calc_text_width(const char* text, float height, bool formatted = false,
		bool word_wrapping = false, float line_width = 0.f, int* line_count = nullptr, int caret_index = -1, int max_len = -1);

	float draw_text_artist(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height,
		float x, float y, float* color, bool formatted = false, float display_width = 0.f, 
		float display_height = 0.f, float scroll_x = 0.f, float scroll_y = 0.f, bool word_wrapping = false, int caret_index = -1, params_t* params = nullptr);

	float draw_text(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height,
		float x, float y, float* color, float* outline_color = nullptr, bool formatted = false, 
		float display_width = 0.f, float display_height = 0.f, float scroll_x = 0.f, float scroll_y = 0.f, bool word_wrapping = false,
		int caret_index = -1, params_t* params = nullptr);

	float draw_text_with_cursor(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, int cursor,
		float height, float x, float y, float* color, float* outline_color = nullptr, bool formatted = false,
		float display_width = 0.f, params_t* params = nullptr);

	void draw_box(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float width, 
		float height, float* color, float* outline_color = nullptr, float outline_thickness = 1.f, params_t* params = nullptr);

	void draw_material(game::fox::gr::dg::plugins::Draw2DRenderer* instance, unsigned int material,
		float x, float y, float width,
		float height, float* color, params_t* params = nullptr);

	void add_stencil(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float width,
		float height);

	void remove_stencil(game::fox::gr::dg::plugins::Draw2DRenderer* instance);

	void on_frame(const std::function<void(game::fox::gr::dg::plugins::Draw2DRenderer*)>& cb);
}
