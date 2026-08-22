#pragma once

#include "../vars.hpp"
#include "utils.hpp"
#include "game/game.hpp"

namespace renderer
{
	extern vars::var_ptr r_custom_text_rendering;

	using draw_cb_t = void(__fastcall*)(game::fox::gr::dg::plugins::Draw2DRenderer*);

	enum draw_priority_t : std::int32_t
	{
		// affected by idroid
		priority_game_begin = 0,
		priority_game_end = 79,
		priority_game = 79,

		// only ingame
		priority_hud_begin = 80,
		priority_hud_end = 170,
		priority_hud = 170,

		// mixed
		priority_3_begin = 199,
		priority_3_end = 209,
		priority_3 = 209,

		// menus
		priority_menu_begin = 210,
		priority_menu_end = 254,
		priority_menu = 254,

		// always
		priority_topmost = 255,
	};

	struct draw2d_t
	{
		game::fox::gr::Draw2D instance;
		draw_cb_t callback;
	};

	float calc_text_width_artist(const char* text, float height, bool formatted = false,
		bool word_wrapping = false, float line_width = 0.f, int* line_count = nullptr, int caret_index = -1, int max_len = -1);

	float calc_text_width(const char* text, float height, bool formatted = false,
		bool word_wrapping = false, float line_width = 0.f, int* line_count = nullptr, int caret_index = -1, int max_len = -1);

	float calc_text_width(const wchar_t* text, float height, bool formatted = false,
		bool word_wrapping = false, float line_width = 0.f, int* line_count = nullptr, int caret_index = -1, int max_len = -1);

	float draw_text_artist(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height,
		float x, float y, float* color, bool formatted = false, float display_width = 0.f, 
		float display_height = 0.f, float scroll_x = 0.f, float scroll_y = 0.f, bool word_wrapping = false, int caret_index = -1, params_t* params = nullptr);

	float draw_text(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height,
		float x, float y, float* color, float* outline_color = nullptr, bool formatted = false, 
		float display_width = 0.f, float display_height = 0.f, float scroll_x = 0.f, float scroll_y = 0.f, bool word_wrapping = false,
		int caret_index = -1, params_t* params = nullptr);

	float draw_text(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const wchar_t* text, float height,
		float x, float y, float* color, float* outline_color = nullptr, bool formatted = false,
		float display_width = 0.f, float display_height = 0.f, float scroll_x = 0.f, float scroll_y = 0.f, bool word_wrapping = false,
		int caret_index = -1, params_t* params = nullptr);

	float draw_text_with_cursor(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, int cursor,
		float height, float x, float y, float* color, float* outline_color = nullptr, bool formatted = false,
		float display_width = 0.f, params_t* params = nullptr);

	float draw_text_with_cursor(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const wchar_t* text, int cursor,
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

	std::unique_ptr<draw2d_t> register_draw(const draw_cb_t cb, const std::int32_t priority = priority_topmost);
}
