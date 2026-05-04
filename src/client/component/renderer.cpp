#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "renderer.hpp"

#include <utils/hook.hpp>
#include <utils/memory.hpp>

namespace renderer
{
	namespace
	{
		std::vector<std::function<void(game::fox::gr::dg::plugins::Draw2DRenderer*)>> render_callbacks;

		unsigned short float_to_half(float value)
		{
			game::fox::gr::_fp16 values16{};
			game::Vectormath::Aos::Vector4 values32{};
			values32.values[0] = value;
			game::fox::gr::ConvertFloat4toHalf4(&values16, &values32);
			return values16.v[0];
		}

		float half_to_float(unsigned short value)
		{
			game::fox::gr::_fp16 values16{};
			game::Vectormath::Aos::Vector4 values32{};
			values16.v[0] = value;
			game::fox::gr::ConvertHalf4toFloat4(&values32, &values16);
			return values32.values[0];
		}

		struct hsv_color
		{
			unsigned char h;
			unsigned char s;
			unsigned char v;
		};

		void hsv_to_rgb(const hsv_color hsv, float* color)
		{
#define SET_RESULT(r, g, b) \
			color[0] = static_cast<float>(r) / 255.f; \
			color[1] = static_cast<float>(g) / 255.f; \
			color[2] = static_cast<float>(b) / 255.f; \

			if (hsv.s == 0)
			{
				SET_RESULT(hsv.v, hsv.v, hsv.v);
				return;
			}

			// converting to 16 bit to prevent overflow
			const unsigned int h = hsv.h;
			const unsigned int s = hsv.s;
			const unsigned int v = hsv.v;

			const auto region = static_cast<uint8_t>(h / 43);
			const auto remainder = (h - (region * 43)) * 6;

			const auto p = static_cast<uint8_t>((v * (255 - s)) >> 8);
			const auto q = static_cast<uint8_t>(
				(v * (255 - ((s * remainder) >> 8))) >> 8);
			const auto t = static_cast<uint8_t>(
				(v * (255 - ((s * (255 - remainder)) >> 8))) >> 8);

			switch (region)
			{
			case 0:
				SET_RESULT(v, t, p);
				break;
			case 1:
				SET_RESULT(q, v, p);
				break;
			case 2:
				SET_RESULT(p, v, t);
				break;
			case 3:
				SET_RESULT(p, q, v);
				break;
			case 4:
				SET_RESULT(t, p, v);
				break;
			default:
				SET_RESULT(v, p, q);
				break;
			}
#undef SET_RESULT
		}

		int get_milliseconds()
		{
			const auto now = std::chrono::high_resolution_clock::now();
			const auto ms_epoch = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
			return static_cast<int>(ms_epoch);
		}

		void set_color_code(char c, float* color)
		{
			switch (c)
			{
			case '0': // black
				color[0] = 0.f;
				color[1] = 0.f;
				color[2] = 0.f;
				break;
			case '1': // red
				color[0] = 0.85f;
				color[1] = 0.f;
				color[2] = 0.f;
				break;
			case '2': // green
				color[0] = 0.52f;
				color[1] = 0.75f;
				color[2] = 0.f;
				break;
			case '3': // yellow
				color[0] = 1.f;
				color[1] = 0.68f;
				color[2] = 0.13f;
				break;
			case '4': // blue
				color[0] = 0.f;
				color[1] = 0.52f;
				color[2] = 0.75f;
				break;
			case '5': // light blue
				color[0] = 0.12f;
				color[1] = 0.77f;
				color[2] = 1.f;
				break;
			case '6': // pink
				color[0] = 0.59f;
				color[1] = 0.31f;
				color[2] = 0.86f;
				break;
			case '7': // white
				color[0] = 1.f;
				color[1] = 1.f;
				color[2] = 1.f;
				break;
			case '8': // team solid
				color[0] = 0.29f;
				color[1] = 0.74f;
				color[2] = 0.88f;
				break;
			case '9': // team liquid
				color[0] = 1.0f;
				color[1] = 0.5f;
				color[2] = 0.3f;
				break;
			case ':':
			{
				hsv_to_rgb({static_cast<uint8_t>((get_milliseconds() / 100) % 256), 255, 255}, color);
				break;
			}
			}
		}

		void execute_push(game::fox::gr::dg::plugins::Draw2DRenderer* instance, unsigned char flags)
		{
			game::fox::gr::Packet2DPush packet{};
			packet.flags = flags;
			game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DPush(instance, &packet);
		}

		void execute_pop(game::fox::gr::dg::plugins::Draw2DRenderer* instance, unsigned char flags)
		{
			game::fox::gr::Packet2DPop packet{};
			packet.flags = flags;
			game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DPop(instance, &packet);
		}

		void set_translation(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float z, unsigned char flags)
		{
			game::fox::gr::Packet2DTranslation packet{};
			packet.x = x;
			packet.y = y;
			packet.z = z;
			packet.flags = flags;
			game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DTranslation(instance, &packet);
		}

		void set_scale(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float z, unsigned char flags)
		{
			game::fox::gr::Packet2DScale packet{};
			packet.x = x;
			packet.y = y;
			packet.z = z;
			packet.flags = flags;
			game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DScale(instance, &packet);
		}

		void set_color(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float* color)
		{
			game::fox::gr::Packet2DColor packet{};
			game::Vectormath::Aos::Vector4 rgba{};
			rgba.values[0] = color[0];
			rgba.values[1] = color[1];
			rgba.values[2] = color[2];
			rgba.values[3] = color[3];
			game::fox::gr::ConvertFloat4toHalf4(&packet.rgba, &rgba);
			game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DColor(instance, &packet);
		}

		void set_matrix(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float* v1, float* v2, float* quat, unsigned char flags)
		{
			game::fox::gr::Packet2DMatrix packet{};
			std::memcpy(packet.v1, v1, sizeof(float[3]));
			std::memcpy(packet.v2, v2, sizeof(float[3]));
			std::memcpy(packet.quat, quat, sizeof(float[4]));
			packet.flags = flags;
			game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DMatrix(instance, &packet);
		}

		void set_material(game::fox::gr::dg::plugins::Draw2DRenderer* instance, game::fox::gr::Material* material)
		{
			game::fox::gr::Packet2DMaterial packet{};
			packet.material = material;
			game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DMaterial(instance, &packet);
		}

		void add_string(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height)
		{
			game::fox::gr::Packet2DString packet{};

			packet.glyphUnk = float_to_half(0.f);
			packet.glyphHeight = float_to_half(height);
			packet.glyphWidth = float_to_half(1.f);
			packet.glyphSpacing = float_to_half(-1.f);
			packet.flags |= 0x80;
			packet.fontType = 1;
			packet.a9 = 0;

			game::fox::gr::dg::StringFontMetricsCache metrics{};
			packet.fontMetricsCache = &metrics;

			game::fox::gr::InitMetrics(packet.fontMetricsCache, text, 0, 1);
			game::fox::gr::dg::plugins::Draw2DRenderer_::ExecuteOnly_Packet2DString(instance, &packet);

			game::fox::FreeAnnotated(packet.fontMetricsCache->glyphs, 0x5000F);
		}

		float add_string_custom(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, int length, float height, float* color = nullptr,
			float start_x = 0.f, float start_y = 0.f)
		{
			game::fox::gr::dg::StringFontMetricsCache metrics{};
			game::fox::gr::InitMetrics(&metrics, text, 0, 1);

			const auto count = std::min(static_cast<int>(metrics.count), length);

			const auto _0 = gsl::finally([&]
			{
				game::fox::FreeAnnotated(metrics.glyphs, 0x5000F);
			});

			const auto width = 1.f * height;
			const auto spacing = -1.f;

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

			auto offset_x = start_x;
			auto offset_y = start_y;

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

			struct vertex_buffer
			{
				float v[6][6];
			};

			auto vertices = reinterpret_cast<vertex_buffer*>(game::fox::gr::dg::DynamicVertexBuffer_::GetBuffer(
				instance->parameters->vertexBuffer, &instance->buffer, &instance->size, 144 * count));
			std::memset(vertices, 0, sizeof(vertex_buffer) * count);

			for (auto i = 0; i < count; i++)
			{
				game::fox::gr::dg::FontTextureMetrics font_metrics{};
				game::fox::gr::dg::FontSystem_::CalculateMetrics(&font_metrics, &instance->glyphData[metrics.glyphs[i]], pixel_width, pixel_height,
					1.f / static_cast<float>(metrics.a2));

				const auto x1 = font_metrics.f7 * width + offset_x;
				const auto x2 = font_metrics.f5 * width + x1;
				const auto y1 = -1.f * (font_metrics.f8 * height) + offset_y;
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

				offset_x += (font_metrics.f9 * width) + spacing;
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

			game::fox::gr::dg::plugins::Draw2DRenderer_::DrawVertices(instance, 2, 24, 6 * count);
			return offset_x - start_x;
		}

		void add_box(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float z, float width, float height)
		{
			game::fox::gr::Packet2DBox packet{};

			packet.f1 = float_to_half(x);
			packet.f2 = float_to_half(y);
			packet.f3 = float_to_half(z);
			packet.f4 = float_to_half(width);
			packet.f5 = float_to_half(height);

			game::fox::Color color{};
			color.values[0] = 1.f;
			color.values[1] = 1.f;
			color.values[2] = 1.f;
			color.values[3] = 1.f;

			packet.color = game::fox::Color_::EncodeUInt32RGBA(&color);

			game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DBox(instance, &packet);
		}

		void set_cull_mode_alpha(game::fox::gr::dg::plugins::Draw2DRenderer* instance, char cull_mode, char alpha)
		{
			game::fox::gr::dg::plugins::Draw2DRenderer_::SetCullMode(instance, cull_mode);
			game::fox::gr::dg::plugins::Draw2DRenderer_::SetAlpha(instance, alpha);
		}

		void set_position(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y)
		{
			float v1[3]{};
			float v2[3]{};
			float quat[4]{};

			v1[0] = 0.1f;
			v1[1] = 0.1f;
			v1[2] = 0.0f;

			v2[0] = -64.f + x * 0.1f;
			v2[1] = 36.f - y * 0.1f;
			v2[2] = -2.5f;

			quat[0] = 1.f;
			quat[1] = 0.f;
			quat[2] = 0.f;
			quat[3] = 0.f;

			set_matrix(instance, v1, v2, quat, 2);
		}

		void draw_box_internal(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float width, float height, float* color)
		{
			set_position(instance, 0.f, 0.f);
			set_material(instance, nullptr);
			set_cull_mode_alpha(instance, 2, 1);
			set_color(instance, color);
			add_box(instance, x, y, 0.f, width, height);
		}

		float draw_text_internal_formatted(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height, float x, float y, float* color)
		{
			set_position(instance, x, y + height);
			set_material(instance, nullptr);
			set_cull_mode_alpha(instance, 2, 1);
			set_color(instance, color);

			auto c = text;
			auto len = 0;

			float string_color[4]{};
			string_color[0] = 1.f;
			string_color[1] = 1.f;
			string_color[2] = 1.f;
			string_color[3] = 1.f;

			auto offset_x = 0.f;
			auto offset_y = 0.f;

			const auto draw_current = [&](int skip_count)
			{
				offset_x += add_string_custom(instance, text, len, height, string_color, offset_x, offset_y);
				text += len + skip_count;
				len = 0;
				c += skip_count;
			};

			while (*c != '\0')
			{
				if (c[0] == '^' && (c[1] >= '0' && c[1] <= '9' || c[1] == ':'))
				{
					const auto color_index = c[1];
					draw_current(2);
					set_color_code(color_index, string_color);
					continue;
				}
				else if (c[0] == '\n')
				{
					draw_current(1);
					offset_x = 0;
					offset_y += height;
					continue;
				}
				else if (c[0] == '\t')
				{
					draw_current(1);
					offset_x += height * 2;
					continue;
				}
			
				++len;
				++c;
			}
			
			if (len > 0)
			{
				offset_x += add_string_custom(instance, text, len, height, string_color, offset_x, offset_y);
			}

			return offset_x;
		}

		float draw_text_internal(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height, float x, float y, float* color)
		{
			set_position(instance, x, y + height);
			set_material(instance, nullptr);
			set_cull_mode_alpha(instance, 2, 1);
			set_color(instance, color);
			return add_string_custom(instance, text, 0xFFFFF, height);
		}

		utils::hook::detour execute_draw_hook;
		void execute_draw_stub(game::fox::gr::dg::plugins::Draw2DRenderer* instance, game::fox::gr::dg::BuildDraw2DParameters* parameters)
		{
			execute_draw_hook.invoke<void>(instance, parameters);
			instance->parameters = parameters;

			const auto system = *game::fox::gr::dg::FontSystem_::m_instance;
			const auto chars = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

			if (system != nullptr)
			{
				game::fox::gr::dg::FontSystem_::UnRegisterString(system, chars, 1);
				game::fox::gr::dg::FontSystem_::RegisterString(system, chars, 1);
			}

			for (const auto& cb : render_callbacks)
			{
				cb(instance);
			}

			instance->parameters = nullptr;
		}
	}

	float get_text_width(const char* text, float height)
	{
		float width{};
		game::fox::gr::Font_::GetStringWidth(text, &width, false, 1);
		return width * height;
	}

	float draw_text(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height, 
		float x, float y, float* color, float* outline_color, bool formatted)
	{
		const auto fn = formatted
			? draw_text_internal_formatted
			: draw_text_internal;

		if (outline_color != nullptr)
		{
			fn(instance, text, height, x + 0.5f, y + 0.5f, outline_color);
		}
		
		return fn(instance, text, height, x, y, color);
	}

	float draw_text_with_cursor(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, int cursor, 
		float height, float x, float y, float* color, float* outline_color, bool formatted)
	{
		static char buffer[0x2000]{};
		std::memset(buffer, 0, sizeof(buffer));

		const auto show_cursor = ((get_milliseconds() % 500) > 500 / 2);
		const auto len = std::strlen(text);

		std::memcpy(buffer, text, cursor);
		std::memcpy(&buffer[cursor + 1], &text[cursor], len - cursor);
		buffer[cursor] = show_cursor ? '_' : ' ';

		return draw_text(instance, buffer, height, x, y, color, outline_color, formatted);
	}

	void draw_box(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float width,
		float height, float* color, float* outline_color, float outline_thickness)
	{
		draw_box_internal(instance, x, y, width, height, color);
		
		if (outline_color != nullptr)
		{
			draw_box_internal(instance, x - outline_thickness, y, outline_thickness, height, outline_color);
			draw_box_internal(instance, x + width, y, outline_thickness, height, outline_color);
		
			draw_box_internal(instance, x - outline_thickness, y, width + 2 * outline_thickness, outline_thickness, outline_color);
			draw_box_internal(instance, x - outline_thickness, y + height, width + 2 * outline_thickness, outline_thickness, outline_color);
		}
	}

	void on_frame(const std::function<void(game::fox::gr::dg::plugins::Draw2DRenderer*)>& cb)
	{
		render_callbacks.emplace_back(cb);
	}

	class component final : public component_interface
	{
	public:
		void start() override
		{
			execute_draw_hook.create(SELECT_VALUE(0x143DC6CF0, 0x14A1E36B0, 0x143E18FF0, 0x149234EB0), execute_draw_stub);
		}
	};
}

REGISTER_COMPONENT(renderer::component)
