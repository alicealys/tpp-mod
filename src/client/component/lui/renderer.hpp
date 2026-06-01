#pragma once

namespace lui::renderer
{
	struct draw_command;

	enum draw_type_t
	{
		CMD_NONE = 0,
		CMD_DRAW_BOX = 1,
		CMD_DRAW_TEXT = 2,
		CMD_DRAW_TEXT_CURSOR = 3,
	};

	struct draw_command
	{
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
		std::uint32_t material;
	};

	struct draw_text_command : draw_command
	{
		char text[256];
		float outline_color[4];
		float display_width;
		float display_height;
		bool formatted;
		bool use_word_wrapping;
		bool artist_font;
	};

	struct draw_text_cursor_command : draw_command
	{
		char text[256];
		float outline_color[4];
		float display_width;
		int cursor;
		bool formatted;
	};

	void add_draw_material(const std::uint32_t material, float x, float y, float width, float height, float* color, float rotation);
	void add_draw_text(const char* text, float height, float x, float y, float* color, float* outline_color, 
		bool formatted, float display_width, float display_height, bool use_word_wrapping = false, float rotation = 0.f, bool artist_font = false);
	void add_draw_text_with_cursor(const char* text, float height, float x, float y, float* color, float* outline_color,
		bool formatted, float display_width, float rotation = 0.f, int cursor = -1);

	void load();
	void end();
}
