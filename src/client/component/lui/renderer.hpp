#pragma once

namespace lui::renderer
{
	struct draw_command;

	enum draw_type_t
	{
		CMD_NONE = 0,
		CMD_DRAW_BOX = 1,
		CMD_DRAW_TEXT = 2,
	};

	struct draw_command
	{
		draw_command* next;
		draw_type_t type;
		size_t size;
		float x;
		float y;
		float width;
		float height;
		float rotation;
		float color[4];
	};

	struct draw_box_command : draw_command
	{
		game::fox::gr::Material* material;
		std::uint32_t texture;
		float uv[4];
	};

	struct draw_text_command : draw_command
	{
		char text[256];
		float outline_color[4];
		bool formatted;
		float display_width;
		float display_height;
		bool use_word_wrapping;
		bool artist_font;
	};

	void add_draw_material(game::fox::gr::Material* material, const std::uint32_t texture, float x, float y, float width, float height, float* color, float rotation, float* uv);
	void add_draw_text(const char* text, float height, float x, float y, float* color, float* outline_color, 
		bool formatted, float display_width, float display_height, bool use_word_wrapping = false, float rotation = 0.f, bool artist_font = false);
}
