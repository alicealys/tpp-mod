#pragma once

#include "game/game.hpp"

namespace renderer
{
	struct artist_font_data
	{
		game::Vectormath::Aos::Vector2 xy[255 * 4];
		game::Vectormath::Aos::Vector2 uv[255 * 4];
		float widths[255];
	};

	struct artist_font_glyph
	{
		float xy[4][2];
		float uv[4][2];
		float width;
	};

	struct glyph_info_t
	{
		game::fox::gr::dg::FontTextureMetrics metrics;
		unsigned int texture_handle;
	};

	struct font_data_t
	{
		bool loaded;
		game::fox::gr::dg::_TextureGlyphData system_font_glyphs[255]{};
		artist_font_glyph artist_font_gylphs[255]{};
		game::fox::gr::Texture* artist_font_texture;
		glyph_info_t wide_char_glyphs[0xFFFF]{};
		float artist_font_height;
	};

	struct custom_font_metrics_t
	{
		game::fox::gr::dg::StringFontMetricsCache native;
		wchar_t* string;
		int str_len;
	};

	extern font_data_t font_data;

	bool is_char_printable(const char c);
	bool is_char_printable(const wchar_t c);
	bool is_system_font(int font_type);
}
