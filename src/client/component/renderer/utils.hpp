#pragma once

#include "../vars.hpp"
#include "game/game.hpp"

namespace renderer
{
	struct hsv_color
	{
		unsigned char h;
		unsigned char s;
		unsigned char v;
	};

	struct vertex_buffer
	{
		float v[6][6];
	};

	struct params_t
	{
		float rotation;
		float perspective[5];
	};

	unsigned short float_to_half(float value);
	float half_to_float(unsigned short value);

	void hsv_to_rgb(const hsv_color hsv, float* color);
	std::uint64_t get_milliseconds();

	bool get_color_code(char c, float* color);
	bool get_color_code(wchar_t c, float* color);

	int get_language_code();
	float get_font_scaling();
	float get_font_y_offset();
	float get_font_spacing();
	int get_font_type();

	void execute_push(game::fox::gr::dg::plugins::Draw2DRenderer* instance, unsigned char flags);
	void execute_pop(game::fox::gr::dg::plugins::Draw2DRenderer* instance, unsigned char flags);
	void set_translation(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float z, unsigned char flags);
	void set_scale(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float z, unsigned char flags);
	void set_color(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float* color);
	void set_matrix(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float* v1, float* v2, float* quat, unsigned char flags);
	void set_material(game::fox::gr::dg::plugins::Draw2DRenderer* instance, game::fox::gr::Material* material);
	void set_material(game::fox::gr::dg::plugins::Draw2DRenderer* instance, unsigned int resource_id);
	void set_texture(game::fox::gr::dg::plugins::Draw2DRenderer* instance, game::fox::gr::Texture* texture);
	void set_texture(game::fox::gr::dg::plugins::Draw2DRenderer* instance, unsigned int resource_id);
	void set_stencil(game::fox::gr::dg::plugins::Draw2DRenderer* instance,
		unsigned char flags, unsigned char a1, unsigned char a2, unsigned char a3, unsigned char a4, unsigned char a5, unsigned char a6, int a9);
	void set_clear_stencil(game::fox::gr::dg::plugins::Draw2DRenderer* instance, int a1);
	void add_string(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height);
	void add_box(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float z, float width, float height);
	void add_box2(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float width, float height, float* uv = nullptr);
	void set_cull_mode(game::fox::gr::dg::plugins::Draw2DRenderer* instance, char cull_mode);
	void set_alpha(game::fox::gr::dg::plugins::Draw2DRenderer* instance, char alpha);
	void set_cull_mode_alpha(game::fox::gr::dg::plugins::Draw2DRenderer* instance, char cull_mode, char alpha);
	void set_cmd_31(game::fox::gr::dg::plugins::Draw2DRenderer* instance, unsigned char flags);
	void set_rotation(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float angle);
	void set_position(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y);
	void set_perspective(game::fox::gr::dg::plugins::Draw2DRenderer* instance, params_t* params);
}
