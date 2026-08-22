#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "draw.hpp"
#include "fonts.hpp"
#include "../scheduler.hpp"
#include "../console.hpp"

#include <utils/hook.hpp>

namespace renderer
{
	vars::var_ptr r_custom_text_rendering;

	namespace
	{
		vars::var_ptr r_draw_priority_test;

		constexpr const auto custom_draw2d_flag = (1 << 8);

		void set_other_params(game::fox::gr::dg::plugins::Draw2DRenderer* instance, params_t* params)
		{
			if (params != nullptr)
			{
				set_rotation(instance, params->rotation);
			}

			set_perspective(instance, params);
		}

		float add_string_custom_artist(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, int length, float height, float* color = nullptr,
			float start_x = 0.f, float start_y = 0.f, float offset_x = 0.f, float offset_y = 0.f, 
			bool formatted = false, bool word_wrapping = false, float line_width = 0.f, int caret_index = -1)
		{
			if (length < 0)
			{
				length = static_cast<int>(std::strlen(text));
			}

			set_texture(instance, font_data.artist_font_texture);

			game::fox::Color current_color{};
			current_color.values[0] = 1.f;
			current_color.values[1] = 1.f;
			current_color.values[2] = 1.f;
			current_color.values[3] = 1.f;

			if (color != nullptr)
			{
				current_color.values[0] = color[0];
				current_color.values[1] = color[1];
				current_color.values[2] = color[2];
				current_color.values[3] = color[3];
			}

			auto color_int = game::fox::Color_::EncodeUInt32RGBA(&current_color);

			offset_y += 2.f;

			const auto scale = height / font_data.artist_font_height;

			for (auto i = 0; i < length; i++)
			{
				const auto glyph = font_data.artist_font_gylphs[text[i]];

				if (formatted)
				{
					if (text[i] == '^' && get_color_code(text[i + 1], current_color.values))
					{
						color_int = game::fox::Color_::EncodeUInt32RGBA(&current_color);
						++i;
						continue;
					}

					switch (text[i])
					{
					case '\n':
						offset_x = 0.f;
						offset_y += height;
						continue;
					case '\t':
						offset_x += font_data.artist_font_gylphs[' '].width * 4.f;
						continue;
					}
				}

				if (word_wrapping && offset_x >= line_width)
				{
					offset_x = 0.f;
					offset_y += height;
				}

				game::fox::gr::Packet2DTriangleStrip<4> triangle_strip{};
				triangle_strip.vertices[0].color = color_int;
				triangle_strip.vertices[0].v[0] = float_to_half(glyph.xy[0][0] * scale + offset_x + start_x);
				triangle_strip.vertices[0].v[1] = float_to_half(-glyph.xy[0][1] * scale + offset_y + start_y);
				triangle_strip.vertices[0].v[2] = float_to_half(0.f);
				triangle_strip.vertices[0].v[3] = float_to_half(glyph.uv[0][0]);
				triangle_strip.vertices[0].v[4] = float_to_half(glyph.uv[0][1]);

				triangle_strip.vertices[1].color = color_int;
				triangle_strip.vertices[1].v[0] = float_to_half(glyph.xy[1][0] * scale + offset_x + start_x);
				triangle_strip.vertices[1].v[1] = float_to_half(-glyph.xy[1][1] * scale + offset_y + start_y);
				triangle_strip.vertices[1].v[2] = float_to_half(0.f);
				triangle_strip.vertices[1].v[3] = float_to_half(glyph.uv[1][0]);
				triangle_strip.vertices[1].v[4] = float_to_half(glyph.uv[1][1]);

				triangle_strip.vertices[2].color = color_int;
				triangle_strip.vertices[2].v[0] = float_to_half(glyph.xy[2][0] * scale + offset_x + start_x);
				triangle_strip.vertices[2].v[1] = float_to_half(-glyph.xy[2][1] * scale + offset_y + start_y);
				triangle_strip.vertices[2].v[2] = float_to_half(0.f);
				triangle_strip.vertices[2].v[3] = float_to_half(glyph.uv[2][0]);
				triangle_strip.vertices[2].v[4] = float_to_half(glyph.uv[2][1]);

				triangle_strip.vertices[3].color = color_int;
				triangle_strip.vertices[3].v[0] = float_to_half(glyph.xy[3][0] * scale + offset_x + start_x);
				triangle_strip.vertices[3].v[1] = float_to_half(-glyph.xy[3][1] * scale + offset_y + start_y);
				triangle_strip.vertices[3].v[2] = float_to_half(0.f);
				triangle_strip.vertices[3].v[3] = float_to_half(glyph.uv[3][0]);
				triangle_strip.vertices[3].v[4] = float_to_half(glyph.uv[3][1]);

				if (i != caret_index)
				{
					offset_x += glyph.width * scale;
				}

				game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DTriangleStrip<4>(instance, &triangle_strip);
			}
			
			return offset_x;
		}

		float add_string_custom(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, int length, float height, float* color,
			float& offset_x, float& offset_y, float start_x, float start_y, bool word_wrapping = false, float line_width = 0.f, int caret_index = -1)
		{
			if (length < 0)
			{
				length = static_cast<int>(std::strlen(text));
			}

			const auto width = 1.f * height;
			const auto spacing = get_font_spacing();

			float pixel_width{};
			float pixel_height{};

			const auto font_system = *game::fox::gr::dg::FontSystem_::m_instance;
			if (font_system != nullptr)
			{
				game::fox::gr::dg::FontSystem_::GetHalfPixelWH(font_system, &pixel_width, &pixel_height);
			}

			float scaling[2]{};
			scaling[0] = (4.f * pixel_height) * 2.f;
			scaling[1] = (4.f * pixel_width) * 2.f;

			const auto font_offset = get_font_y_offset();

			game::Vectormath::Aos::Vector4 color_vec{};
			if (color != nullptr)
			{
				color_vec.values[0] = color[0];
				color_vec.values[1] = color[1];
				color_vec.values[2] = color[2];
				color_vec.values[3] = 0.f;
			}
			else
			{
				color_vec.values[0] = 1.f;
				color_vec.values[1] = 1.f;
				color_vec.values[2] = 1.f;
				color_vec.values[3] = 0.f;
			}

			auto vertices = reinterpret_cast<vertex_buffer*>(game::fox::gr::dg::DynamicVertexBuffer_::GetBuffer(
				instance->parameters->vertexBuffer, &instance->buffer, &instance->size, 144 * length));
			std::memset(vertices, 0, sizeof(vertex_buffer) * length);

			auto result_width = 0.f;

			for (auto i = 0; i < length; i++)
			{
				game::fox::gr::dg::FontTextureMetrics font_metrics{};
				const auto char_idx = static_cast<unsigned char>(text[i]);
				const auto glyph = &font_data.system_font_glyphs[char_idx];
				if (glyph->character == 0)
				{
					continue;
				}

				game::fox::gr::dg::FontSystem_::CalculateMetrics(&font_metrics, glyph, pixel_width, pixel_height, 1.f / 60.f);

				if (word_wrapping && offset_x + font_metrics.f9 * width >= line_width)
				{
					result_width = 0.f;
					offset_x = 0.f;
					offset_y += height;
				}

				const auto x1 = font_metrics.f7 * width + offset_x + start_x;
				const auto x2 = font_metrics.f5 * width + x1;
				const auto y1 = -1.f * (font_metrics.f8 * height) + offset_y + font_offset + start_y;
				const auto y2 = font_metrics.f6 * height + y1;

				vertices->v[0][0] = x1;
				vertices->v[0][1] = y1;
				vertices->v[0][2] = 0.f;
				*reinterpret_cast<int*>(&vertices->v[0][3]) = -1;
				vertices->v[0][4] = font_metrics.f1;
				vertices->v[0][5] = font_metrics.f2;

				vertices->v[1][0] = x2;
				vertices->v[1][1] = y1;
				vertices->v[1][2] = 0.f;
				*reinterpret_cast<int*>(&vertices->v[1][3]) = -1;
				vertices->v[1][4] = font_metrics.f3;
				vertices->v[1][5] = font_metrics.f2;

				vertices->v[2][0] = x1;
				vertices->v[2][1] = y2;
				vertices->v[2][2] = 0.f;
				*reinterpret_cast<int*>(&vertices->v[2][3]) = -1;
				vertices->v[2][4] = font_metrics.f1;
				vertices->v[2][5] = font_metrics.f4;

				vertices->v[3][0] = x2;
				vertices->v[3][1] = y2;
				vertices->v[3][2] = 0.f;
				*reinterpret_cast<int*>(&vertices->v[3][3]) = -1;
				vertices->v[3][4] = font_metrics.f3;
				vertices->v[3][5] = font_metrics.f4;

				vertices->v[4][0] = vertices->v[2][0];
				vertices->v[4][1] = vertices->v[2][1];
				vertices->v[4][2] = 0.f;
				vertices->v[4][3] = vertices->v[2][3];
				vertices->v[4][4] = vertices->v[2][4];
				vertices->v[4][5] = vertices->v[2][5];

				vertices->v[5][0] = vertices->v[1][0];
				vertices->v[5][1] = vertices->v[1][1];
				vertices->v[5][2] = 0.f;
				vertices->v[5][3] = vertices->v[1][3];
				vertices->v[5][4] = vertices->v[1][4];
				vertices->v[5][5] = vertices->v[1][5];
				++vertices;

				if (i != caret_index)
				{
					switch (text[i])
					{
					case '\t':
						offset_x += width * 2.f;
						result_width += width * 2.f;
						break;
					case '\n':
						result_width = 0.f;
						offset_x = 0.f;
						offset_y += height;
						break;
					default:
						offset_x += (font_metrics.f9 * width) + spacing;
						result_width += (font_metrics.f9 * width) + spacing;
						break;
					}
				}
			}

			auto font_texture_handle = 0;
			if (font_system != nullptr)
			{
				font_texture_handle = game::fox::gr::dg::FontSystem_::GetFontTextureHandle(font_system);
			}

			game::fox::gr::dg::CommandBuffer_::SetTexture(instance->commandBuffer, 8, font_texture_handle);

			game::Vectormath::Aos::Vector4 vec1{};

			vec1.values[0] = 1.f * pixel_width;
			vec1.values[1] = 1.f * pixel_height;
			vec1.values[2] = 1.f;
			vec1.values[3] = 0.f;

			game::fox::gr::dg::CommandBuffer_::SetVector(instance->commandBuffer, 35, &color_vec, 0);
			game::fox::gr::dg::CommandBuffer_::SetVector(instance->commandBuffer, 181, &vec1, 0);

			game::fox::gr::dg::plugins::Draw2DRenderer_::DrawVertices(instance, 2, 24, 6 * length);
			return result_width;
		}

		float add_wstring_custom(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const wchar_t* text, int length, float base_height, float* color,
			float& offset_x, float& offset_y, float start_x, float start_y, bool word_wrapping = false, float line_width = 0.f, int caret_index = -1)
		{
			if (length < 0)
			{
				length = 0xFFFF;
			}

			game::Vectormath::Aos::Vector4 color_vec{};
			if (color != nullptr)
			{
				color_vec.values[0] = color[0];
				color_vec.values[1] = color[1];
				color_vec.values[2] = color[2];
				color_vec.values[3] = 0.f;
			}
			else
			{
				color_vec.values[0] = 1.f;
				color_vec.values[1] = 1.f;
				color_vec.values[2] = 1.f;
				color_vec.values[3] = 0.f;
			}

			auto result_width = 0.f;

			for (auto i = 0; i < length; i++)
			{
				if (text[i] == 0)
				{
					break;
				}

				auto vertices = reinterpret_cast<vertex_buffer*>(game::fox::gr::dg::DynamicVertexBuffer_::GetBuffer(
					instance->parameters->vertexBuffer, &instance->buffer, &instance->size, 144));
				std::memset(vertices, 0, sizeof(vertex_buffer));

				const auto idx = static_cast<std::uint16_t>(text[i]);
				const auto glyph_info = &font_data.wide_char_glyphs[idx];

				const auto height = base_height * 1.f;
				const auto width = 1.f * height;

				if (word_wrapping && offset_x + glyph_info->metrics.f9 * width >= line_width)
				{
					result_width = 0.f;
					offset_x = 0.f;
					offset_y += height;
				}

				if (text[i] == '^' && get_color_code(text[i + 1], color_vec.values))
				{
					++i;
					continue;
				}

				const auto x1 = glyph_info->metrics.f7 * width + offset_x + start_x;
				const auto x2 = glyph_info->metrics.f5 * width + x1;
				const auto y1 = -1.f * (glyph_info->metrics.f8 * height) + offset_y + start_y;
				const auto y2 = glyph_info->metrics.f6 * height + y1;

				vertices->v[0][0] = x1;
				vertices->v[0][1] = y1;
				vertices->v[0][2] = 0.f;
				*reinterpret_cast<int*>(&vertices->v[0][3]) = -1;
				vertices->v[0][4] = glyph_info->metrics.f1;
				vertices->v[0][5] = glyph_info->metrics.f2;

				vertices->v[1][0] = x2;
				vertices->v[1][1] = y1;
				vertices->v[1][2] = 0.f;
				*reinterpret_cast<int*>(&vertices->v[1][3]) = -1;
				vertices->v[1][4] = glyph_info->metrics.f3;
				vertices->v[1][5] = glyph_info->metrics.f2;

				vertices->v[2][0] = x1;
				vertices->v[2][1] = y2;
				vertices->v[2][2] = 0.f;
				*reinterpret_cast<int*>(&vertices->v[2][3]) = -1;
				vertices->v[2][4] = glyph_info->metrics.f1;
				vertices->v[2][5] = glyph_info->metrics.f4;

				vertices->v[3][0] = x2;
				vertices->v[3][1] = y2;
				vertices->v[3][2] = 0.f;
				*reinterpret_cast<int*>(&vertices->v[3][3]) = -1;
				vertices->v[3][4] = glyph_info->metrics.f3;
				vertices->v[3][5] = glyph_info->metrics.f4;

				vertices->v[4][0] = vertices->v[2][0];
				vertices->v[4][1] = vertices->v[2][1];
				vertices->v[4][2] = 0.f;
				vertices->v[4][3] = vertices->v[2][3];
				vertices->v[4][4] = vertices->v[2][4];
				vertices->v[4][5] = vertices->v[2][5];

				vertices->v[5][0] = vertices->v[1][0];
				vertices->v[5][1] = vertices->v[1][1];
				vertices->v[5][2] = 0.f;
				vertices->v[5][3] = vertices->v[1][3];
				vertices->v[5][4] = vertices->v[1][4];
				vertices->v[5][5] = vertices->v[1][5];
				++vertices;

				if (i != caret_index)
				{
					switch (text[i])
					{
					case '\t':
						offset_x += width * 2.f;
						result_width += width * 2.f;
						break;
					case '\n':
						result_width = 0.f;
						offset_x = 0.f;
						offset_y += height;
						break;
					default:
						offset_x += (glyph_info->metrics.f9 * width);
						result_width += (glyph_info->metrics.f9 * width);
						break;
					}
				}

				game::fox::gr::dg::CommandBuffer_::SetTexture(instance->commandBuffer, 8, glyph_info->texture_handle);
				game::fox::gr::dg::CommandBuffer_::SetVector(instance->commandBuffer, 35, &color_vec, 0);
				game::fox::gr::dg::plugins::Draw2DRenderer_::DrawVertices(instance, 2, 24, 6);
			}

			return result_width;
		}
		
		float draw_text_internal_formatted(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height, float x, float y, float* color,
			float display_width, float display_height, float scroll_x, float scroll_y, bool word_wrapping, int caret_index, params_t* params)
		{
			height *= get_font_scaling();

			set_position(instance, 0.f, 0.f);
			set_other_params(instance, params);

			auto start_x = x;
			auto start_y = y + height;

			const auto has_stencil = display_width != 0.f && display_height != 0.f;
			if (has_stencil)
			{
				add_stencil(instance, x, y, display_width, display_height);
				start_x -= scroll_x;
				start_y -= scroll_y;
			}

			set_material(instance, nullptr);
			set_cull_mode_alpha(instance, 2, 1);

			float color_default[4]{};
			color_default[0] = 1.f;
			color_default[1] = 1.f;
			color_default[2] = 1.f;
			color_default[3] = 1.f;

			if (color == nullptr)
			{
				set_color(instance, color_default);
			}
			else
			{
				set_color(instance, color);
			}

			float string_color[4]{};
			string_color[0] = 1.f;
			string_color[1] = 1.f;
			string_color[2] = 1.f;
			string_color[3] = 1.f;

			auto c = text;
			auto len = 0;

			auto offset_x = 0.f;
			auto offset_y = 0.f;

			const auto draw_current = [&](int skip_count)
			{
				add_string_custom(instance, text, len, height, string_color, offset_x, offset_y, start_x, start_y,
					word_wrapping, display_width, caret_index);
				text += len + skip_count;
				len = 0;
				c += skip_count;
			};

			while (*c != '\0')
			{
				float next_color[3]{};
				if (c[0] == '^' && get_color_code(c[1], next_color))
				{
					draw_current(2);
					std::memcpy(string_color, next_color, sizeof(float[3]));
					continue;
				}

				++len;
				++c;
			}

			if (len > 0)
			{
				add_string_custom(instance, text, len, height, string_color, offset_x, offset_y, start_x, start_y,
					word_wrapping, display_width, caret_index);
			}

			if (has_stencil)
			{
				remove_stencil(instance);
			}

			return offset_x;
		}

		float draw_text_internal(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height, float x, float y, float* color, 
			float display_width, float display_height, float scroll_x, float scroll_y, bool word_wrapping, int caret_index, params_t* params)
		{
			height *= get_font_scaling();

			set_position(instance, 0.f, 0.f);
			set_other_params(instance, params);

			auto start_x = x;
			auto start_y = y + height;

			const auto has_stencil = display_width != 0.f && display_height != 0.f;
			if (has_stencil)
			{
				add_stencil(instance, x, y, display_width, display_height);
				start_x -= scroll_x;
				start_y -= scroll_y;
			}

			set_material(instance, nullptr);
			set_cull_mode_alpha(instance, 2, 1);

			float color_default[4]{};
			color_default[0] = 1.f;
			color_default[1] = 1.f;
			color_default[2] = 1.f;
			color_default[3] = 1.f;

			if (color == nullptr)
			{
				set_color(instance, color_default);
			}
			else
			{
				set_color(instance, color);
			}

			auto offset_x = 0.f;
			auto offset_y = 0.f;

			const auto width = add_string_custom(instance, text, -1, height, nullptr, offset_x, offset_y, 
				start_x, start_y, word_wrapping, display_width, caret_index);
			if (has_stencil)
			{
				remove_stencil(instance);
			}

			return width;
		}

		float draw_wtext_internal(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const wchar_t* text, float height, float x, float y, float* color,
			float display_width, float display_height, float scroll_x, float scroll_y, bool word_wrapping, int caret_index, params_t* params)
		{
			set_position(instance, 0.f, 0.f);
			set_other_params(instance, params);

			auto start_x = x;
			auto start_y = y + height;

			const auto has_stencil = display_width != 0.f && display_height != 0.f;
			if (has_stencil)
			{
				add_stencil(instance, x, y, display_width, display_height);
				start_x -= scroll_x;
				start_y -= scroll_y;
			}

			set_material(instance, nullptr);
			set_cull_mode_alpha(instance, 2, 1);

			float color_default[4]{};
			color_default[0] = 1.f;
			color_default[1] = 1.f;
			color_default[2] = 1.f;
			color_default[3] = 1.f;

			if (color == nullptr)
			{
				set_color(instance, color_default);
			}
			else
			{
				set_color(instance, color);
			}

			auto offset_x = 0.f;
			auto offset_y = 0.f;

			const auto width = add_wstring_custom(instance, text, -1, height, nullptr, offset_x, offset_y,
				start_x, start_y, word_wrapping, display_width, caret_index);
			if (has_stencil)
			{
				remove_stencil(instance);
			}

			return width;
		}

		void draw_box_internal(game::fox::gr::dg::plugins::Draw2DRenderer* instance, unsigned int material,
			float x, float y, float width, float height, float* color, params_t* params)
		{
			set_position(instance, 0.f, 0.f);
			set_material(instance, nullptr);
			set_cull_mode_alpha(instance, 2, 1);
			set_color(instance, color);
			set_other_params(instance, params);

			if (material != 0)
			{
				set_material(instance, material);
			}

			add_box2(instance, x, y, width, height);
		}

		void prepare_view(game::fox::gr::dg::plugins::Draw2DRenderer* instance)
		{
			game::fox::gr::dg::plugins::Draw2DRenderer_::InitRenderingState(instance);
			game::fox::gr::dg::plugins::Draw2DRenderer_::InitPerObject(instance);

			instance->viewVec2.values[0] = 1280.f;
			instance->viewVec2.values[1] = 720.f;
			instance->viewVec2.values[2] = 1.f;
			instance->viewVec2.values[3] = 0.f;

			game::fox::gr::dg::plugins::Draw2DRenderer_::UpdateViewMapping(instance);
			game::fox::gr::dg::plugins::Draw2DRenderer_::SetDepthMode(instance, 2);
		}

		utils::hook::detour draw_object_hook;
		void draw_object_stub(game::fox::gr::dg::plugins::Draw2DRenderer* instance, game::fox::gr::Draw2D* draw)
		{
			if ((draw->flags & custom_draw2d_flag) != 0)
			{
				const auto custom = reinterpret_cast<draw2d_t*>(draw);
				prepare_view(instance);
				custom->callback(instance);
				return;
			}

			draw_object_hook.invoke<void>(instance, draw);
		}

		utils::hook::detour execute_packet2d_string_hook;
		void execute_packet2d_string_stub(game::fox::gr::dg::plugins::Draw2DRenderer* instance, game::fox::gr::Packet2DString* string)
		{
			if (!r_custom_text_rendering->current.enabled() || !is_system_font(string->fontType))
			{
				return execute_packet2d_string_hook.invoke<void>(instance, string);
			}

			const auto custom_metrics = reinterpret_cast<custom_font_metrics_t*>(string->fontMetricsCache);
			if (custom_metrics->string == nullptr)
			{
				return execute_packet2d_string_hook.invoke<void>(instance, string);
			}

			game::Vectormath::Aos::Vector4 color_vec{};
			color_vec.values[0] = 1.f;
			color_vec.values[1] = 1.f;
			color_vec.values[2] = 1.f;
			color_vec.values[3] = 0.f;

			auto spacing = half_to_float(string->glyphSpacing);
			auto scaling_x = 1.f;
			auto scaling_y = 1.f;
			auto offset_x = 0.f;
			auto offset_y = 0.f;

			switch (get_language_code())
			{
			case 'npj':
				spacing += 1.f;
				break;
			case 'sur':
				scaling_x = 1.1f;
				offset_y = 1.f;
				break;
			default:
				spacing += 1.f;
				break;
			}

			auto height = half_to_float(string->glyphHeight);
			auto unk = half_to_float(string->glyphUnk);
			auto width = half_to_float(string->glyphWidth) * height;

			width *= scaling_x;
			height *= scaling_y;

			if (unk > 0.f)
			{
				auto v1 = unk / height;
				auto v2 = static_cast<float>(string->fontMetricsCache->stringWidth);
				if (v1 < v2)
				{
					width *= v1 / v2;
				}
			}

			float white_color[4]{};
			white_color[0] = 1.f;
			white_color[1] = 1.f;
			white_color[2] = 1.f;
			white_color[3] = 1.f;

			for (auto i = 0; i < custom_metrics->str_len; i++)
			{
				auto vertices = reinterpret_cast<vertex_buffer*>(game::fox::gr::dg::DynamicVertexBuffer_::GetBuffer(
					instance->parameters->vertexBuffer, &instance->buffer, &instance->size, 144));
				std::memset(vertices, 0, sizeof(vertex_buffer));

				const auto c = custom_metrics->string[i];
				const auto c_next = custom_metrics->string[i + 1];
				const auto glyph_info = &font_data.wide_char_glyphs[c];

				float new_color[4]{};
				if (c == '^' && get_color_code(static_cast<wchar_t>(c_next), new_color))
				{
					const auto current_color = instance->commandBuffer->shadowConstantRegister->unk1[3].buffers[1].vec;
					const auto product = current_color[0] * current_color[1] * current_color[2];

					if (product > 0.f)
					{
						current_color[0] = new_color[0];
						current_color[1] = new_color[1];
						current_color[2] = new_color[2];
					}

					++i;
					continue;
				}

				const auto x1 = glyph_info->metrics.f7 * width + offset_x;
				const auto x2 = glyph_info->metrics.f5 * width + x1;
				const auto y1 = -1.f * (glyph_info->metrics.f8 * height) + offset_y;
				const auto y2 = glyph_info->metrics.f6 * height + y1;

				vertices->v[0][0] = x1;
				vertices->v[0][1] = y1;
				vertices->v[0][2] = 0.f;
				*reinterpret_cast<int*>(&vertices->v[0][3]) = -1;
				vertices->v[0][4] = glyph_info->metrics.f1;
				vertices->v[0][5] = glyph_info->metrics.f2;

				vertices->v[1][0] = x2;
				vertices->v[1][1] = y1;
				vertices->v[1][2] = 0.f;
				*reinterpret_cast<int*>(&vertices->v[1][3]) = -1;
				vertices->v[1][4] = glyph_info->metrics.f3;
				vertices->v[1][5] = glyph_info->metrics.f2;

				vertices->v[2][0] = x1;
				vertices->v[2][1] = y2;
				vertices->v[2][2] = 0.f;
				*reinterpret_cast<int*>(&vertices->v[2][3]) = -1;
				vertices->v[2][4] = glyph_info->metrics.f1;
				vertices->v[2][5] = glyph_info->metrics.f4;

				vertices->v[3][0] = x2;
				vertices->v[3][1] = y2;
				vertices->v[3][2] = 0.f;
				*reinterpret_cast<int*>(&vertices->v[3][3]) = -1;
				vertices->v[3][4] = glyph_info->metrics.f3;
				vertices->v[3][5] = glyph_info->metrics.f4;

				vertices->v[4][0] = vertices->v[2][0];
				vertices->v[4][1] = vertices->v[2][1];
				vertices->v[4][2] = 0.f;
				vertices->v[4][3] = vertices->v[2][3];
				vertices->v[4][4] = vertices->v[2][4];
				vertices->v[4][5] = vertices->v[2][5];

				vertices->v[5][0] = vertices->v[1][0];
				vertices->v[5][1] = vertices->v[1][1];
				vertices->v[5][2] = 0.f;
				vertices->v[5][3] = vertices->v[1][3];
				vertices->v[5][4] = vertices->v[1][4];
				vertices->v[5][5] = vertices->v[1][5];
				++vertices;

				offset_x += (glyph_info->metrics.f9 * width) + spacing;

				game::fox::gr::dg::CommandBuffer_::SetTexture(instance->commandBuffer, 8, glyph_info->texture_handle);
				game::fox::gr::dg::CommandBuffer_::SetVector(instance->commandBuffer, 35, &color_vec, 0);
				game::fox::gr::dg::plugins::Draw2DRenderer_::DrawVertices(instance, 2, 24, 6);
			}
		}

		template <int Priority, int Index>
		void add_priority_test()
		{
			static wchar_t text[0x200]{};
			_snwprintf_s(text, 0x200, _TRUNCATE, L"PRIORITY %i", Priority);
			register_draw([](game::fox::gr::dg::plugins::Draw2DRenderer* instance)
			{
				float color[4]{};
				color[0] = 1.f;
				color[1] = 1.f;
				color[2] = 1.f;
				color[3] = 1.f;

				draw_text(instance, text, 15.f, 100.f, static_cast<float>(Index) * 20.f + 100.f, color);
			}, Priority);
		}
	}

	float calc_text_width_artist(const char* text, float height, bool formatted,
		bool word_wrapping, float line_width, int* line_count, int caret_index, int max_len)
	{
		if (text == nullptr)
		{
			return 0.f;
		}

		if (max_len < 0)
		{
			max_len = static_cast<int>(std::strlen(text));
		}

		auto offset_x = 0.f;
		auto offset_y = 0.f;
		auto prev_offset = 0.f;
		const auto scale = height / font_data.artist_font_height;

		for (auto i = 0; i < max_len; i++)
		{
			if (text[i] == '^' && get_color_code(text[i + 1], nullptr))
			{
				++i;
				continue;
			}

			switch (text[i])
			{
			case '\n':
				offset_x = 0.f;
				offset_y += height;
				continue;
			case '\t':
				offset_x += font_data.artist_font_gylphs[' '].width * 4.f;
				continue;
			}

			if (word_wrapping && offset_x > line_width)
			{
				prev_offset = std::max(prev_offset, offset_x);
				offset_x = 0.f;
				offset_y += height;
			}

			if (i != caret_index)
			{
				offset_x += font_data.artist_font_gylphs[text[i]].width * scale;
			}
		}

		return std::max(prev_offset, offset_x);
	}

	float calc_text_width(const char* text, float height, bool formatted, bool word_wrapping, 
		float line_width, int* line_count, int caret_index, int max_len)
	{
		if (text == nullptr)
		{
			return 0.f;
		}

		const auto count = static_cast<int>(std::strlen(text));
		if (max_len == -1)
		{
			max_len = count;
		}

		const auto width = 1.f * height * get_font_scaling();
		const auto spacing = get_font_spacing();

		float pixel_width{};
		float pixel_height{};

		const auto font_system = *game::fox::gr::dg::FontSystem_::m_instance;
		if (font_system != nullptr)
		{
			game::fox::gr::dg::FontSystem_::GetHalfPixelWH(font_system, &pixel_width, &pixel_height);
		}

		auto offset_x = 0.f;
		auto prev_offset = 0.f;

		for (auto i = 0; i < std::min(max_len, count); i++)
		{
			game::fox::gr::dg::FontTextureMetrics font_metrics{};
			const auto char_idx = static_cast<unsigned char>(text[i]);
			game::fox::gr::dg::FontSystem_::CalculateMetrics(&font_metrics, &font_data.system_font_glyphs[char_idx], pixel_width, pixel_height, 1.f / 60.f);

			if (formatted && text[i] == '^' && get_color_code(text[i + 1], nullptr))
			{
				++i;
				continue;
			}

			if (i == caret_index)
			{
				continue;
			}

			if (word_wrapping && offset_x + font_metrics.f9 * width >= line_width) 
			{
				prev_offset = std::max(prev_offset, offset_x);
				offset_x = 0.f;

				if (line_count != nullptr)
				{
					(*line_count)++;
				}
			}

			switch (text[i])
			{
			case '\t':
				offset_x += width * 2.f;
				break;
			case '\n':
				prev_offset = std::max(prev_offset, offset_x);
				offset_x = 0.f;
				break;
			default:
				offset_x += (font_metrics.f9 * width) + spacing;
				break;
			}
		}

		return std::max(prev_offset, offset_x);
	}

	float calc_text_width(const wchar_t* text, float height, bool formatted, bool word_wrapping,
		float line_width, int* line_count, int caret_index, int max_len)
	{
		if (text == nullptr)
		{
			return 0.f;
		}

		const auto count = static_cast<int>(std::wcslen(text));
		if (max_len == -1)
		{
			max_len = count;
		}

		auto offset_x = 0.f;
		auto prev_offset = 0.f;

		for (auto i = 0; i < std::min(max_len, count); i++)
		{
			const auto width = 1.f * height;
			const auto char_idx = static_cast<std::uint16_t>(text[i]);
			const auto glyph = &font_data.wide_char_glyphs[char_idx];

			if (formatted && text[i] == '^' && get_color_code(text[i + 1], nullptr))
			{
				++i;
				continue;
			}

			if (i == caret_index)
			{
				continue;
			}

			if (word_wrapping && offset_x + glyph->metrics.f9 * width >= line_width)
			{
				prev_offset = std::max(prev_offset, offset_x);
				offset_x = 0.f;

				if (line_count != nullptr)
				{
					(*line_count)++;
				}
			}

			switch (text[i])
			{
			case '\t':
				offset_x += width * 2.f;
				break;
			case '\n':
				prev_offset = std::max(prev_offset, offset_x);
				offset_x = 0.f;
				break;
			default:
				offset_x += (glyph->metrics.f9 * width);
				break;
			}
		}

		return std::max(prev_offset, offset_x);
	}

	float draw_text_artist(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height,
		float x, float y, float* color, bool formatted, float display_width, float display_height,
		float scroll_x, float scroll_y, bool word_wrapping, int caret_index, params_t* params)
	{
		set_position(instance, 0.f, 0.f);
		set_other_params(instance, params);

		auto start_x = x;
		auto start_y = y + height;

		const auto has_stencil = display_width != 0.f && display_height != 0.f;
		if (has_stencil)
		{
			add_stencil(instance, x, y, display_width, display_height);
			start_x -= scroll_x;
			start_y -= scroll_y;
		}

		set_material(instance, nullptr);
		set_cull_mode_alpha(instance, 2, 1);

		float color_default[4]{};
		color_default[0] = 1.f;
		color_default[1] = 1.f;
		color_default[2] = 1.f;
		color_default[3] = 1.f;

		if (color == nullptr)
		{
			set_color(instance, color_default);
		}
		else
		{
			set_color(instance, color);
		}

		const auto width = add_string_custom_artist(instance, text, -1, height, nullptr, start_x, start_y, 0.f, 0.f, 
			formatted, word_wrapping, display_width, caret_index);
		if (has_stencil)
		{
			remove_stencil(instance);
		}
		return width;
	}

	float draw_text(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height, 
		float x, float y, float* color, float* outline_color, bool formatted, float display_width, float display_height, 
		float scroll_x, float scroll_y, bool word_wrapping, int caret_index, params_t* params)
	{
		const auto fn = formatted
			? draw_text_internal_formatted
			: draw_text_internal;

		if (outline_color != nullptr)
		{
			fn(instance, text, height, x + 0.5f, y + 0.5f, outline_color, display_width, display_height, scroll_x, scroll_y, word_wrapping, caret_index, params);
		}
		
		return fn(instance, text, height, x, y, color, display_width, display_height, scroll_x, scroll_y, word_wrapping, caret_index, params);
	}

	float draw_text(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const wchar_t* text, float height,
		float x, float y, float* color, float* outline_color, bool formatted, float display_width, float display_height,
		float scroll_x, float scroll_y, bool word_wrapping, int caret_index, params_t* params)
	{
		if (outline_color != nullptr)
		{
			draw_wtext_internal(instance, text, height, x + 0.5f, y + 0.5f, outline_color, display_width, display_height, scroll_x, scroll_y, word_wrapping, caret_index, params);
		}

		return draw_wtext_internal(instance, text, height, x, y, color, display_width, display_height, scroll_x, scroll_y, word_wrapping, caret_index, params);
	}

	float draw_text_with_cursor(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, int cursor,
		float height, float x, float y, float* color, float* outline_color, bool formatted, float display_width, params_t* params)
	{
		static char buffer[0x2000]{};
		std::memset(buffer, 0, sizeof(buffer));

		const auto show_cursor = ((get_milliseconds() % 500) > 250);
		const auto len = std::strlen(text);

		cursor = std::min(cursor, static_cast<int>(len));

		std::memcpy(buffer, text, cursor);
		std::memcpy(&buffer[cursor + 1], &text[cursor], len - cursor);

		auto scroll_x = 0.f;

		buffer[cursor] = show_cursor ? '_' : ' ';

		auto caret_index = cursor;
		if (cursor == len)
		{
			caret_index = -1;
		}

		auto max_cursor = cursor - 1;
		if (max_cursor < 0)
		{
			max_cursor = 0;
		}

		const auto text_width = calc_text_width(buffer, height, formatted, false, 0.f, nullptr, caret_index, -1);
		const auto text_width_to_cursor = calc_text_width(buffer, height, formatted, false, 0.f, nullptr, caret_index, max_cursor);

		if (text_width > display_width)
		{
			scroll_x = text_width - display_width;

			if (text_width - text_width_to_cursor > display_width)
			{
				scroll_x -= text_width - text_width_to_cursor - display_width;
			}
		}

		return draw_text(instance, buffer, height, x, y, color, outline_color, formatted, display_width, display_width, scroll_x, 0.f, false, caret_index, params);
	}

	float draw_text_with_cursor(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const wchar_t* text, int cursor,
		float height, float x, float y, float* color, float* outline_color, bool formatted, float display_width, params_t* params)
	{
		static wchar_t buffer[0x2000]{};
		std::memset(buffer, 0, sizeof(buffer));

		const auto show_cursor = ((get_milliseconds() % 500) > 500 / 2);
		const auto len = std::wcslen(text);

		cursor = std::min(cursor, static_cast<int>(len));

		std::memcpy(buffer, text, cursor * sizeof(wchar_t));
		std::memcpy(&buffer[cursor + 1], &text[cursor], (len - cursor) * sizeof(wchar_t));

		auto scroll_x = 0.f;

		buffer[cursor] = show_cursor ? '_' : ' ';

		auto caret_index = cursor;
		if (cursor == len)
		{
			caret_index = -1;
		}

		auto max_cursor = cursor - 1;
		if (max_cursor < 0)
		{
			max_cursor = 0;
		}

		const auto text_width = calc_text_width(buffer, height, formatted, false, 0.f, nullptr, caret_index, -1);
		const auto text_width_to_cursor = calc_text_width(buffer, height, formatted, false, 0.f, nullptr, caret_index, max_cursor);

		if (text_width > display_width)
		{
			scroll_x = text_width - display_width;

			if (text_width - text_width_to_cursor > display_width)
			{
				scroll_x -= text_width - text_width_to_cursor - display_width;
			}
		}

		return draw_text(instance, buffer, height, x, y, color, outline_color, formatted, display_width, display_width, scroll_x, 0.f, false, caret_index, params);
	}

	void draw_box(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float width,
		float height, float* color, float* outline_color, float outline_thickness, params_t* params)
	{
		draw_box_internal(instance, 0, x, y, width, height, color, params);
		
		if (outline_color != nullptr)
		{
			draw_box_internal(instance, 0, x - outline_thickness, y, outline_thickness, height, outline_color, params);
			draw_box_internal(instance, 0, x + width, y, outline_thickness, height, outline_color, params);
		
			draw_box_internal(instance, 0, x - outline_thickness, y - outline_thickness, width + 2 * outline_thickness, outline_thickness, outline_color, params);
			draw_box_internal(instance, 0, x - outline_thickness, y + height, width + 2 * outline_thickness, outline_thickness, outline_color, params);
		}
	}

	void draw_material(game::fox::gr::dg::plugins::Draw2DRenderer* instance, unsigned int material, float x, float y, float width,
		float height, float* color, params_t* params)
	{
		draw_box_internal(instance, material, x, y, width, height, color, params);
	}

	void add_stencil(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float width, float height)
	{
		set_cmd_31(instance, 0);
		set_clear_stencil(instance, 0);
		set_alpha(instance, 0);
		set_stencil(instance, 1, 7, 1, 255, 0, 0, 2, -1);
		add_box(instance, x, y, 0.f, width, height);
		set_cmd_31(instance, 15);
		set_stencil(instance, 1, 2, 1, 255, 0, 0, 0, -1);
	}

	void remove_stencil(game::fox::gr::dg::plugins::Draw2DRenderer* instance)
	{
		prepare_view(instance);
		set_stencil(instance, 0, 7, 1, 255, 0, 0, 0, -1);
	}

	std::unique_ptr<draw2d_t> register_draw(const draw_cb_t cb, const std::int32_t priority)
	{
		const auto default_scene = game::fox::gr::Scene_::GetDefaultScene();
		if (default_scene == nullptr)
		{
			console::error("could not register draw, no scene\n");
			return nullptr;
		}

		auto draw_2d = std::make_unique<draw2d_t>();
		auto draw_2d_base = reinterpret_cast<game::fox::gr::Draw2D*>(draw_2d.get());

		game::fox::gr::Draw2D_::Draw2D_(draw_2d_base);
		draw_2d->callback = cb;
		draw_2d_base->priority = priority;
		draw_2d_base->flags |= custom_draw2d_flag;

		game::fox::gr::Scene_::Queue(default_scene, draw_2d_base);

		return std::move(draw_2d);
	}

	namespace draw
	{
		class component final : public component_interface
		{
		public:
			void pre_load() override
			{
				r_custom_text_rendering = vars::register_bool("r_custom_text_rendering", true, vars::var_flag_saved,
					"enable custom text rendering (fixes languages not displaying + color codes)");

				r_draw_priority_test = vars::register_bool("r_draw_priority_test", false, vars::var_flag_saved | vars::var_flag_latched,
					"enable draw priority test overlay");
			}

			void start() override
			{
				if (game::environment::is_dedi())
				{
					return;
				}

				execute_packet2d_string_hook.create(
					game::fox::gr::dg::plugins::Draw2DRenderer_::ExecuteOnly_Packet2DString.get(), execute_packet2d_string_stub);

				draw_object_hook.create(SELECT_VALUE(0x1402E6BE0, 0x140BD94A0, 0x0, 0x0), draw_object_stub);
			}

			void on_game_initialized() override
			{
				if (!r_draw_priority_test->current.enabled())
				{
					return;
				}

				add_priority_test<0, 0>();
				add_priority_test<10, 1>();
				add_priority_test<20, 2>();
				add_priority_test<30, 3>();
				add_priority_test<40, 4>();
				add_priority_test<50, 5>();
				add_priority_test<60, 6>();
				add_priority_test<70, 7>();
				add_priority_test<80, 8>();
				add_priority_test<90, 9>();
				add_priority_test<100, 10>();
				add_priority_test<110, 11>();
				add_priority_test<120, 12>();
				add_priority_test<130, 13>();
				add_priority_test<140, 14>();
				add_priority_test<150, 15>();
				add_priority_test<160, 16>();
				add_priority_test<170, 17>();
				add_priority_test<180, 18>();
				add_priority_test<190, 19>();
				add_priority_test<200, 20>();
				add_priority_test<210, 21>();
				add_priority_test<220, 22>();
				add_priority_test<230, 23>();
				add_priority_test<240, 24>();
				add_priority_test<250, 25>();
				add_priority_test<255, 26>();
			}
		};
	}
}

REGISTER_COMPONENT(renderer::draw::component)
