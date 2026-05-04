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

		namespace packet
		{
			void push(game::fox::gr::dg::plugins::Draw2DRenderer* instance, unsigned char flags)
			{
				game::fox::gr::Packet2DPush packet{};
				packet.flags = flags;
				game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DPush(instance, &packet);
			}

			void pop(game::fox::gr::dg::plugins::Draw2DRenderer* instance, unsigned char flags)
			{
				game::fox::gr::Packet2DPop packet{};
				packet.flags = flags;
				game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DPop(instance, &packet);
			}

			void translation(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float z, unsigned char flags)
			{
				game::fox::gr::Packet2DTranslation packet{};
				packet.x = x;
				packet.y = y;
				packet.z = z;
				packet.flags = flags;
				game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DTranslation(instance, &packet);
			}

			void scale(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float z, unsigned char flags)
			{
				game::fox::gr::Packet2DScale packet{};
				packet.x = x;
				packet.y = y;
				packet.z = z;
				packet.flags = flags;
				game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DScale(instance, &packet);
			}

			void color(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float* color)
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

			void matrix(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float* v1, float* v2, float* quat, unsigned char flags)
			{
				game::fox::gr::Packet2DMatrix packet{};
				std::memcpy(packet.v1, v1, sizeof(float[3]));
				std::memcpy(packet.v2, v2, sizeof(float[3]));
				std::memcpy(packet.quat, quat, sizeof(float[4]));
				packet.flags = flags;
				game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DMatrix(instance, &packet);
			}

			void material(game::fox::gr::dg::plugins::Draw2DRenderer* instance, game::fox::gr::Material* material)
			{
				game::fox::gr::Packet2DMaterial packet{};
				packet.material = material;
				game::fox::gr::dg::plugins::Draw2DRenderer_::Execute_Packet2DMaterial(instance, &packet);
			}

			void string(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height)
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

			void box(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float z, float width, float height)
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

			void cull_mode_alpha(game::fox::gr::dg::plugins::Draw2DRenderer* instance, char cull_mode, char alpha)
			{
				game::fox::gr::dg::plugins::Draw2DRenderer_::SetCullMode(instance, cull_mode);
				game::fox::gr::dg::plugins::Draw2DRenderer_::SetAlpha(instance, alpha);
			}
		}

		utils::hook::detour execute_draw_hook;
		void execute_draw_stub(game::fox::gr::dg::plugins::Draw2DRenderer* instance, game::fox::gr::dg::BuildDraw2DParameters* parameters)
		{
			execute_draw_hook.invoke<void>(instance, parameters);
			instance->parameters = parameters;

			for (const auto& cb : render_callbacks)
			{
				cb(instance);
			}

			instance->parameters = nullptr;
		}

		void set_matrix(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y)
		{
			float v1[3]{};
			float v2[3]{};
			float quat[4]{};

			v1[0] = 0.1f;
			v1[1] = 0.1f;
			v1[2] = 0.1f;

			v2[0] = -64.f + x * 0.1f;
			v2[1] = 36.f - y * 0.1f;
			v2[2] = -1.f;

			quat[0] = 1.f;
			quat[1] = 0.f;
			quat[2] = 0.f;
			quat[3] = 0.f;

			packet::matrix(instance, v1, v2, quat, 2);
		}

		void draw_box_internal(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float x, float y, float width, float height, float* color)
		{
			set_matrix(instance, 0.f, 0.f);
			packet::material(instance, nullptr);
			packet::cull_mode_alpha(instance, 2, 1);
			packet::color(instance, color);
			packet::box(instance, x, y, 0.f, width, height);
		}

		void draw_text_internal(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height, float x, float y, float* color)
		{
			set_matrix(instance, x, y + height);
			packet::material(instance, nullptr);
			packet::cull_mode_alpha(instance, 2, 1);
			packet::color(instance, color);
			packet::string(instance, text, height);
		}
	}

	float get_text_width(const char* text, float height)
	{
		float width{};
		game::fox::gr::Font_::GetStringWidth(text, &width, false, 1);
		return width * height;
	}

	float draw_text(game::fox::gr::dg::plugins::Draw2DRenderer* instance, const char* text, float height, float x, float y, float* color, float* outline_color)
	{
		const auto system = *game::fox::gr::dg::FontSystem_::m_instance;
		if (system == nullptr)
		{
			return 0.f;
		}

		game::fox::gr::dg::FontSystem_::RegisterString(system, text, 1);

		if (outline_color != nullptr)
		{
			draw_text_internal(instance, text, height, x + 0.5f, y + 0.5f, outline_color);
		}

		draw_text_internal(instance, text, height, x, y, color);

		float width{};
		game::fox::gr::Font_::GetStringWidth(text, &width, false, 1);
		return width * height;
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
