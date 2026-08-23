#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "draw.hpp"
#include "fonts.hpp"
#include "../scheduler.hpp"

#include <utils/hook.hpp>

namespace renderer
{
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

	std::uint64_t get_milliseconds()
	{
		const auto now = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
	}

	bool get_color_code(char c, float* color)
	{
		float out_color[4]{};
		if (color == nullptr)
		{
			color = out_color;
		}

		switch (c)
		{
		case '0': // black
			color[0] = 0.f;
			color[1] = 0.f;
			color[2] = 0.f;
			return true;
		case '1': // red
			color[0] = 1.f;
			color[1] = 0.3f;
			color[2] = 0.2f;
			return true;
		case '2': // green
			color[0] = 0.4f;
			color[1] = 0.9f;
			color[2] = 0.7f;
			return true;
		case '3': // yellow
			color[0] = 1.f;
			color[1] = 0.8f;
			color[2] = 0.2f;
			return true;
		case '4': // purple
			color[0] = 0.72f;
			color[1] = 0.64f;
			color[2] = 0.88f;
			return true;
		case '5': // light blue
			color[0] = 0.57f;
			color[1] = 0.86f;
			color[2] = 0.91f;
			return true;
		case '6': // pink
			color[0] = 0.89f;
			color[1] = 0.59f;
			color[2] = 0.81f;
			return true;
		case '7': // white
			color[0] = 1.f;
			color[1] = 1.f;
			color[2] = 1.f;
			return true;
		case '8': // ally
			color[0] = 0.29f;
			color[1] = 0.74f;
			color[2] = 0.88f;
			return true;
		case '9': // axis
			color[0] = 1.0f;
			color[1] = 0.5f;
			color[2] = 0.3f;
			return true;
		case ':': // rainbow
			hsv_to_rgb({static_cast<uint8_t>((get_milliseconds() / 100) % 256), 255, 255}, color);
			return true;
		case ';': // grey
			color[0] = 0.5f;
			color[1] = 0.5f;
			color[2] = 0.5f;
			return true;
		}

		return false;
	}

	bool get_color_code(wchar_t c, float* color)
	{
		if (c > 255)
		{
			return false;
		}

		return get_color_code(static_cast<char>(c), color);
	}

	int get_language_code()
	{
		const auto lang = game::fox::AssetConfiguration_::GetLanguage();
		return *reinterpret_cast<const int*>(lang->data->buffer);
	}

	float get_font_scaling()
	{
		switch (get_language_code())
		{
		case 'npj':
			return 1.4f;
		case 'sur':
			return 1.1f;
		}

		return 1.f;
	}

	float get_font_y_offset()
	{
		switch (get_language_code())
		{
		case 'npj':
			return 0.f;
		case 'sur':
			return 1.f;
		}

		return 0.f;
	}

	float get_font_spacing()
	{
		switch (get_language_code())
		{
		case 'npj':
			return -1.f;
		case 'sur':
			return -0.3f;
		}

		return -1.f;
	}

	int get_font_type()
	{
		switch (get_language_code())
		{
		case 'npj':
			return 2;
		case 'sur':
			return 1;
		}

		return 1;
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

	void set_material(game::fox::gr::dg::plugins::Draw2DRenderer* instance, unsigned int resource_id)
	{
		game::fox::gr::Material material{};
		material.resource.data = resource_id;
		game::fox::gr::Packet2DMaterial packet{};
		packet.material = &material;
		game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DMaterial(instance, &packet);
	}

	void set_texture(game::fox::gr::dg::plugins::Draw2DRenderer* instance, game::fox::gr::Texture* texture)
	{
		game::fox::gr::Packet2DTexture packet{};

		if (texture != nullptr)
		{
			packet.id = texture->id.data;
		}

		game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DTexture(instance, &packet);
	}

	void set_texture(game::fox::gr::dg::plugins::Draw2DRenderer* instance, unsigned int resource_id)
	{
		game::fox::gr::Packet2DTexture packet{};
		packet.id = resource_id;
		game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DTexture(instance, &packet);
	}

	void set_stencil(game::fox::gr::dg::plugins::Draw2DRenderer* instance,
		unsigned char flags, unsigned char a1, unsigned char a2, unsigned char a3, unsigned char a4, unsigned char a5, unsigned char a6, int a9)
	{
		game::fox::gr::Packet2DStencil packet{};
		packet.flags = flags;
		packet.a1 = a1;
		packet.a2 = a2;
		packet.a3 = a3;
		packet.a4 = a4;
		packet.a5 = a5;
		packet.a6 = a6;
		packet.a9 = a9;
		game::fox::gr::dg::plugins::Draw2DRenderer_::SetStencil(instance, &packet);
	}

	void set_clear_stencil(game::fox::gr::dg::plugins::Draw2DRenderer* instance, int a1)
	{
		game::fox::gr::Packet2DClearStencil packet{};
		packet.a1 = a1;
		game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DClearStencil(instance, &packet);
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

	void add_box2(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float width, float height, float* uv)
	{
		game::fox::Color color{};
		color.values[0] = 1.f;
		color.values[1] = 1.f;
		color.values[2] = 1.f;
		color.values[3] = 1.f;
		const auto color_int = game::fox::Color_::EncodeUInt32RGBA(&color);

		float result_uv[4][2]{};
		if (uv == nullptr)
		{
			result_uv[0][0] = 0.f;
			result_uv[0][1] = 1.f;

			result_uv[1][0] = 0.f;
			result_uv[1][1] = 0.f;

			result_uv[2][0] = 1.f;
			result_uv[2][1] = 1.f;

			result_uv[3][0] = 1.f;
			result_uv[3][1] = 0.f;
		}
		else
		{
			result_uv[0][0] = uv[0];
			result_uv[0][1] = uv[1];

			result_uv[1][0] = uv[0];
			result_uv[1][1] = uv[3];

			result_uv[2][0] = uv[2];
			result_uv[2][1] = uv[1];

			result_uv[3][0] = uv[2];
			result_uv[3][1] = uv[3];
		}

		auto start_x = x + width / 2.f;
		auto start_y = y + height / 2.f;

		game::fox::gr::Packet2DTriangleStrip<4> triangle_strip{};
		triangle_strip.vertices[0].color = color_int;
		triangle_strip.vertices[0].v[0] = float_to_half(start_x + -0.5f * width);
		triangle_strip.vertices[0].v[1] = float_to_half(start_y + 0.5f * height);
		triangle_strip.vertices[0].v[2] = float_to_half(0.f);
		triangle_strip.vertices[0].v[3] = float_to_half(result_uv[0][0]);
		triangle_strip.vertices[0].v[4] = float_to_half(result_uv[0][1]);

		triangle_strip.vertices[1].color = color_int;
		triangle_strip.vertices[1].v[0] = float_to_half(start_x + -0.5f * width);
		triangle_strip.vertices[1].v[1] = float_to_half(start_y + -0.5f * height);
		triangle_strip.vertices[1].v[2] = float_to_half(0.f);
		triangle_strip.vertices[1].v[3] = float_to_half(result_uv[1][0]);
		triangle_strip.vertices[1].v[4] = float_to_half(result_uv[1][1]);

		triangle_strip.vertices[2].color = color_int;
		triangle_strip.vertices[2].v[0] = float_to_half(start_x + 0.5f * width);
		triangle_strip.vertices[2].v[1] = float_to_half(start_y + 0.5f * height);
		triangle_strip.vertices[2].v[2] = float_to_half(0.f);
		triangle_strip.vertices[2].v[3] = float_to_half(result_uv[2][0]);
		triangle_strip.vertices[2].v[4] = float_to_half(result_uv[2][1]);

		triangle_strip.vertices[3].color = color_int;
		triangle_strip.vertices[3].v[0] = float_to_half(start_x + 0.5f * width);
		triangle_strip.vertices[3].v[1] = float_to_half(start_y + -0.5f * height);
		triangle_strip.vertices[3].v[2] = float_to_half(0.f);
		triangle_strip.vertices[3].v[3] = float_to_half(result_uv[3][0]);
		triangle_strip.vertices[3].v[4] = float_to_half(result_uv[3][1]);

		game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DTriangleStrip<4>(instance, &triangle_strip);
	}

	void set_cull_mode(game::fox::gr::dg::plugins::Draw2DRenderer* instance, char cull_mode)
	{
		game::fox::gr::dg::plugins::Draw2DRenderer_::SetCullMode(instance, cull_mode);
	}

	void set_alpha(game::fox::gr::dg::plugins::Draw2DRenderer* instance, char alpha)
	{
		game::fox::gr::dg::plugins::Draw2DRenderer_::SetAlpha(instance, alpha);
	}

	void set_cull_mode_alpha(game::fox::gr::dg::plugins::Draw2DRenderer* instance, char cull_mode, char alpha)
	{
		game::fox::gr::dg::plugins::Draw2DRenderer_::SetCullMode(instance, cull_mode);
		game::fox::gr::dg::plugins::Draw2DRenderer_::SetAlpha(instance, alpha);
	}

	void set_cmd_31(game::fox::gr::dg::plugins::Draw2DRenderer* instance, unsigned char flags)
	{
		const auto flag = 16 * (flags & 0xF);
		instance->renderStateContext->flags1[0] &= 0xF;
		instance->renderStateContext->flags1[1] &= 0xF;
		instance->renderStateContext->flags1[2] &= 0xF;
		instance->renderStateContext->flags1[3] &= 0xF;
		instance->renderStateContext->flags1[4] &= 0xF;
		instance->renderStateContext->flags1[5] &= 0xF;
		instance->renderStateContext->flags1[6] &= 0xF;
		instance->renderStateContext->flags1[7] &= 0xF;

		instance->renderStateContext->flags1[0] |= flag;
		instance->renderStateContext->flags1[1] |= flag;
		instance->renderStateContext->flags1[2] |= flag;
		instance->renderStateContext->flags1[3] |= flag;
		instance->renderStateContext->flags1[4] |= flag;
		instance->renderStateContext->flags1[5] |= flag;
		instance->renderStateContext->flags1[6] |= flag;
		instance->renderStateContext->flags1[7] |= flag;

		instance->renderStateContext->flags3 &= 0xFFFFFF7F;
		instance->renderStateContext->flags2 |= 0x80;
	}

	void set_rotation(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float angle)
	{
		game::fox::gr::Packet2DRotation packet{};
		packet.flags |= 8;

		const auto rad = angle * (std::numbers::pi_v<float> / 180.f);

		packet.quat[0] = 0.f;
		packet.quat[1] = 0.f;
		packet.quat[2] = std::sin(rad / 2.f);
		packet.quat[3] = std::cos(rad / 2.f);

		game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DRotation(instance, &packet);
	}

	void set_position(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y)
	{
		float v1[3]{};
		float v2[3]{};
		float quat[4]{};

		v1[0] = 1.f;
		v1[1] = -1.f;
		v1[2] = 1.f;

		v2[0] = x;
		v2[1] = y;
		v2[2] = 1.f;

		quat[0] = 1.f;
		quat[1] = 0.f;
		quat[2] = 0.f;
		quat[3] = 0.f;

		set_matrix(instance, v1, v2, quat, 2);
	}

	void set_perspective(game::fox::gr::dg::plugins::Draw2DRenderer* instance, params_t* params)
	{
		game::fox::gr::Packet2DPerspective packet{};

		if (params == nullptr)
		{
			packet.f1 = 1.f;
			packet.f2 = 0.f;
			packet.f3 = 0.f;
			packet.f4 = 0.f;
			packet.f5 = 0.f;
		}
		else
		{
			packet.f1 = params->perspective[0];
			packet.f2 = params->perspective[1];
			packet.f3 = params->perspective[2];
			packet.f4 = params->perspective[3];
			packet.f5 = params->perspective[4];
		}

		game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DPerspective(instance, &packet);
	}
}

