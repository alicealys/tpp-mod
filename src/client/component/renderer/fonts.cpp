#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "utils.hpp"
#include "fonts.hpp"
#include "draw.hpp"
#include "../scheduler.hpp"
#include "../vars.hpp"

#include <utils/hook.hpp>

namespace renderer
{
	font_data_t font_data{};

	namespace
	{
		void init_memory_manager(game::fox::gr::dg::MemoryManager* manager, const size_t size)
		{
			const auto aligned_size = (size + 15) & 0xFFFFFFFull;
			const auto buffer = game::fox::KernelAllocAligned(aligned_size, 0x10);

			manager->unk1 = reinterpret_cast<game::fox::gr::dg::MemoryManager_unk1*>(buffer);
			manager->size = aligned_size;

			manager->unk1->unk1 = 0;
			manager->unk1->unk2 = manager->unk1;
			manager->unk1->unk3 = aligned_size;
			manager->unk1->unk4 = 129;
		}

		void destroy_memory_manager(game::fox::gr::dg::MemoryManager* manager)
		{
			game::fox::FreeAnnotated(manager->unk1, 0x5000F);
			std::memset(manager, 0, sizeof(game::fox::gr::dg::MemoryManager));
		}

		void initialize_font_texture_render(game::fox::gr::dg::FontTextureRender* render)
		{
			utils::hook::set<size_t>(SELECT_VALUE(0x1402034DE, 0x140AD359E, 0x0, 0x0) + 2, 0x3DA16AC4E5A3); // force new texture resource
			game::fox::gr::dg::FontTextureRender_::FontTextureRender_(render, 0x800u, 0x400u);
			utils::hook::set<size_t>(SELECT_VALUE(0x1402034DE, 0x140AD359E, 0x0, 0x0) + 2, 0xB8A0BF169F98);
		}

		void destroy_font_texture_render(game::fox::gr::dg::FontTextureRender* render)
		{
			render->imageGlyphManager->__vftable->__destructor(render->imageGlyphManager, 1);
			render->imageAreaManager->__vftable->__destructor(render->imageAreaManager, 1);

			if (render->unk1)
			{
				game::fox::FreeAnnotated(render->unk1, 0);
			}

			if (render->unk2)
			{
				game::fox::FreeAnnotated(render->unk2, 0);
			}

			std::memset(render, 0, sizeof(game::fox::gr::dg::FontTextureRender));
		}

		int load_font_internal(game::fox::gr::dg::FontData* font, int begin, int max_count,
			float scale, char spacing,
			char x_offset, char y_offset)
		{
			game::fox::gr::dg::FontTextureRender render{};
			initialize_font_texture_render(&render);
			const auto _0 = gsl::finally([&]
			{
				destroy_font_texture_render(&render);
			});

			const auto end = std::min(static_cast<short>(begin + max_count), font->glyphCount);

			auto done_count = 0;
			for (auto i = begin; i < end; i++)
			{
				const auto glyph = &font->glyphs[i];
				const auto glyph_info = &font_data.wide_char_glyphs[glyph->character];

				unsigned char area_1 = glyph->width + 2 * (font->spacing + 4);
				unsigned char area_2 = font->unk1_3 + 2 * (font->spacing + 4);
				const auto area_info = game::fox::gr::dg::ImageAreaManager_::AllocateAreaInfo(
					render.imageAreaManager, area_1, area_2, glyph->character);

				if (area_info == nullptr)
				{
					return done_count;
				}

				const auto texture_glyph_data = game::fox::gr::dg::ImageGlyphManager_::RegisterGlyphData(
					render.imageGlyphManager, font, area_info, glyph->character);

				glyph_info->texture_handle = render.fontTextureHandle;
				game::fox::gr::dg::FontTextureRender_::AddTextureRenderOrder(&render, font, glyph, texture_glyph_data);

				texture_glyph_data->width = static_cast<unsigned char>(static_cast<float>(texture_glyph_data->width) * scale);
				texture_glyph_data->height = static_cast<unsigned char>(static_cast<float>(texture_glyph_data->height) * scale);
				texture_glyph_data->horizontalSpace += spacing;
				texture_glyph_data->horizontalShift += x_offset;
				texture_glyph_data->verticalShift += y_offset;

				const auto half_pixel_width = 0.5f / static_cast<float>(render.pixel_width);
				const auto half_pixel_height = 0.5f / static_cast<float>(render.pixel_height);
				game::fox::gr::dg::FontSystem_::CalculateMetrics(&glyph_info->metrics, texture_glyph_data, 
					half_pixel_width, half_pixel_height, 1.f / 60.f);

				++done_count;
			}
			
			game::fox::gr::dg::FontTextureRender_::UpdateRenderGlyphTexture(&render);
			return done_count;
		}

		void load_font(const char* font_name,
			float scale, char spacing,
			char x_offset, char y_offset)
		{
			game::fox::gr::dg::FontData font{};
			game::fox::gr::dg::MemoryManager memory_manager{};

			font.fontIndex = 0;
			font.memoryManager = &memory_manager;
			init_memory_manager(&memory_manager, 0x27D000);

			const auto _0 = gsl::finally([&]
			{
				destroy_memory_manager(&memory_manager);
			});

			if (!game::fox::gr::dg::FontData_::LoadFontData(&font, font_name, nullptr))
			{
				return;
			}

			auto left_count = static_cast<int>(font.glyphCount);
			auto begin = 0;
			while (left_count > 0)
			{
				const auto max_count = std::min(256, left_count);
				const auto count = load_font_internal(&font, begin, max_count, scale, spacing, x_offset, y_offset);
				left_count -= count;
				begin += count;
			}
		}

		void init_fonts()
		{
			load_font("/Assets/tpp/font/KanjiFont.ffnt", 1.2f, -6, -8, 5);
			load_font("/Assets/tpp/font/font_def_jp.ffnt", 1.3f, 4, 0, 5);
			load_font("/Assets/tpp/font/russian.ffnt", 1.0f, -20, -8, 16);
			load_font("/Assets/tpp/font/font_def_ltn.ffnt", 1.f, -3, 0, 0);
		}

		bool init_font_data()
		{
			const auto font_system = *game::fox::gr::dg::FontSystem_::m_instance;
			if (!font_system)
			{
				return false;
			}

			const auto font_type = get_font_type();

			auto ascii_count = 0;
			unsigned char ascii_chars[255]{};

			for (auto i = 32; i < 127; i++)
			{
				ascii_chars[ascii_count++] = static_cast<unsigned char>(i);
			}

			for (auto i = 160; i < 254; i++)
			{
				ascii_chars[ascii_count++] = static_cast<unsigned char>(i);
			}

			auto utf8_index = 0;
			char utf8_chars[255 * 4]{};

			for (auto i = 0; i < ascii_count; i++)
			{
				const auto c = ascii_chars[i];
				if (c < 128)
				{
					utf8_chars[utf8_index++] = static_cast<char>(c);
				}
				else
				{
					utf8_chars[utf8_index++] = static_cast<char>(0xC0 | (c >> 6));
					utf8_chars[utf8_index++] = static_cast<char>(0x80 | (c & 0x3F));
				}
			}

			game::fox::gr::dg::FontSystem_::RegisterString(font_system, utf8_chars, font_type);
			game::fox::gr::dg::StringFontMetricsCache metrics{};
			game::fox::gr::InitMetrics(&metrics, utf8_chars, 1, font_type);

			if (metrics.count <= 0)
			{
				return false;
			}

			const auto glyph_data = game::fox::gr::dg::FontSystem_::GetFontTextureGlyphDataTop(font_system);

			for (auto i = 0; i < metrics.count; i++)
			{
				const auto idx = static_cast<unsigned char>(ascii_chars[i]);
				std::memcpy(&font_data.system_font_glyphs[idx], &glyph_data[metrics.glyphs[i]], sizeof(game::fox::gr::dg::_TextureGlyphData));
			}

			const auto font_manager = *game::fox::ui::RawDaemon_::s_fontManager;
			if (font_manager == nullptr)
			{
				return false;
			}

			game::fox::StringId language_code{};
			language_code.id = 0xEF76460534C5; // "eng"

			game::fox::StringId font_name{};
			font_name.id = 0x1829B3B18A01; // "cmn-hud-art-L"

			const auto font_group = game::fox::ui::FontManager_::GetFontGroup(font_manager, language_code, font_name.f.l);
			if (font_group == nullptr || font_group->font == nullptr || font_group->font->texture == nullptr)
			{
				return false;
			}

			font_data.artist_font_texture = font_group->font->texture;

			auto res_width = 0.f;
			auto res_height = 0.f;

			static game::Vectormath::Aos::Vector2 xy[0x200]{};
			static game::Vectormath::Aos::Vector2 uv[0x200]{};

			font_data.artist_font_height = 50.f;

			const auto chars = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
			if (!game::fox::ui::Font_::CreateText(font_group->font, chars, font_data.artist_font_height, font_data.artist_font_height, font_group->spacing,
				xy, uv, &res_width, &res_height))
			{
				return false;
			}

			auto previous_x = 0.f;
			for (auto i = 0; i < 95; i++)
			{
				const auto x = xy[(i * 4) + 3].values[0];
				const auto c = chars[i];

				const auto target_width = font_data.artist_font_height / 2.f;
				const auto width = xy[(i * 4) + 3].values[0] - xy[(i * 4) + 0].values[0];
				auto diff = (target_width - width) / 2.f;
				if (!std::isalnum(c))
				{
					diff = 0.f;
				}

				font_data.artist_font_gylphs[c].xy[0][0] = xy[(i * 4) + 0].values[0] - previous_x + diff;
				font_data.artist_font_gylphs[c].xy[1][0] = xy[(i * 4) + 1].values[0] - previous_x + diff;
				font_data.artist_font_gylphs[c].xy[2][0] = xy[(i * 4) + 2].values[0] - previous_x + diff;
				font_data.artist_font_gylphs[c].xy[3][0] = xy[(i * 4) + 3].values[0] - previous_x + diff;

				font_data.artist_font_gylphs[c].xy[0][1] = xy[(i * 4) + 0].values[1];
				font_data.artist_font_gylphs[c].xy[1][1] = xy[(i * 4) + 1].values[1];
				font_data.artist_font_gylphs[c].xy[2][1] = xy[(i * 4) + 2].values[1];
				font_data.artist_font_gylphs[c].xy[3][1] = xy[(i * 4) + 3].values[1];

				font_data.artist_font_gylphs[c].uv[0][0] = uv[(i * 4) + 0].values[0];
				font_data.artist_font_gylphs[c].uv[1][0] = uv[(i * 4) + 1].values[0];
				font_data.artist_font_gylphs[c].uv[2][0] = uv[(i * 4) + 2].values[0];
				font_data.artist_font_gylphs[c].uv[3][0] = uv[(i * 4) + 3].values[0];

				font_data.artist_font_gylphs[c].uv[0][1] = uv[(i * 4) + 0].values[1];
				font_data.artist_font_gylphs[c].uv[1][1] = uv[(i * 4) + 1].values[1];
				font_data.artist_font_gylphs[c].uv[2][1] = uv[(i * 4) + 2].values[1];
				font_data.artist_font_gylphs[c].uv[3][1] = uv[(i * 4) + 3].values[1];

				font_data.artist_font_gylphs[c].width = target_width;
				previous_x = x;
			}

			return true;
		}

		utils::hook::detour change_language_hook;
		void change_language_stub(void* a1)
		{
			change_language_hook.invoke<void>(a1);
			font_data.loaded = init_font_data();
		}

		void* alloc_font_metrics_stub(int a1, int a2)
		{
			const auto buffer = game::fox::KernelAllocAligned(sizeof(custom_font_metrics_t), 4);
			std::memset(buffer, 0, sizeof(custom_font_metrics_t));
			return buffer;
		}

		void free_font_metrics_stub(custom_font_metrics_t* metrics)
		{
			if (metrics->string != nullptr)
			{
				game::fox::FreeAnnotated(metrics->string, 0x5000F);
				metrics->string = nullptr;
			}
			
			game::fox::FreeAnnotated(metrics, 0x5000F);
		}

		void init_metrics_stub(custom_font_metrics_t* string, const char* text, int a3, int font_type)
		{
			game::fox::gr::InitMetrics(&string->native, text, a3, font_type);

			if (!r_custom_text_rendering->current.enabled() || !is_system_font(font_type))
			{
				return;
			}

			auto count = 0;
			{
				auto c = reinterpret_cast<unsigned char*>(reinterpret_cast<size_t>(text));
				while (*c != 0)
				{
					game::fox::Utf8ToUtf32(&c);
					++count;
				};
			}

			auto buffer = game::fox::KernelAllocAligned(count * 2, 4);
			string->string = reinterpret_cast<wchar_t*>(buffer);

			auto c = reinterpret_cast<unsigned char*>(reinterpret_cast<size_t>(text));
			auto idx = 0;
			do
			{
				string->string[idx++] = static_cast<wchar_t>(game::fox::Utf8ToUtf32(&c));
			} while (*c != 0);

			string->str_len = count;
		}

		utils::hook::detour get_string_width_hook;
		int get_string_width_stub(void* a1, game::fox::gr::dg::FontData* a2, unsigned char* text, float* length, char font_type)
		{
			if (!r_custom_text_rendering->current.enabled() || !is_system_font(a2->fontIndex))
			{
				return get_string_width_hook.invoke<int>(a1, a2, text, length, font_type);
			}

			auto c = text;
			auto idx = 0;
			*length = 0.f;

			auto spacing = 0.f;
			auto scaling = 1.f;
			switch (get_language_code())
			{
			case 'npj':
				spacing = 3.f;
				scaling = 1.f;
				break;
			case 'sur':
				scaling = 1.1f;
				spacing = 0.f;
				break;
			default:
				spacing = 3.f;
				scaling = 1.f;
				break;
			}

			while (*c != 0)
			{
				if (c[0] == '^' && get_color_code(static_cast<char>(c[1]), nullptr))
				{
					c += 2;
					continue;
				}

				const auto char_idx = game::fox::Utf8ToUtf32(&c);
				if (char_idx >= 0xFFFF || !is_char_printable(static_cast<wchar_t>(char_idx)))
				{
					continue;
				}

				*length += font_data.wide_char_glyphs[char_idx].metrics.f9 * scaling + spacing * (1.f / 60.f);
				++idx;
			}

			return idx;
		}

		utils::hook::detour get_utf8_count_hook;
		int get_utf8_count_stub(unsigned char* text)
		{
			if (!r_custom_text_rendering->current.enabled())
			{
				return get_utf8_count_hook.invoke<int>(text);
			}

			auto c = text;
			auto idx = 0;
			while (*c != 0)
			{
				if (c[0] == '^' && get_color_code(static_cast<char>(c[1]), nullptr))
				{
					c += 2;
					continue;
				}

				const auto char_idx = game::fox::Utf8ToUtf32(&c);
				if (char_idx >= 0xFFFF || !is_char_printable(static_cast<wchar_t>(char_idx)))
				{
					continue;
				}

				++idx;
			}

			return idx;
		}
	}

	bool is_char_printable(const char c)
	{
		auto idx = static_cast<unsigned char>(c);
		return font_data.system_font_glyphs[idx].character != 0;
	}

	bool is_char_printable(const wchar_t c)
	{
		return iswprint(c) && font_data.wide_char_glyphs[c].texture_handle != 0;
	}

	bool is_system_font(int font_type)
	{
		return font_type == 1 || font_type == 2;
	}

	namespace fonts
	{
		class component final : public component_interface
		{
		public:
			void pre_load() override
			{

			}

			void start() override
			{
				if (game::environment::is_dedi())
				{
					return;
				}

				scheduler::once(init_fonts, scheduler::main);

				utils::hook::call(SELECT_VALUE(0x1402AF515, 0x140BA5CE5, 0x0, 0x0), alloc_font_metrics_stub);
				utils::hook::call(SELECT_VALUE(0x1402AF3C8, 0x140BA5B98, 0x0, 0x0), free_font_metrics_stub);
				utils::hook::call(SELECT_VALUE(0x1402AF52A, 0x140BA5CFA, 0x0, 0x0), init_metrics_stub);
				get_utf8_count_hook.create(SELECT_VALUE(0x140914AA0, 0x140686B60, 0x0, 0x0), get_utf8_count_stub);
				get_string_width_hook.create(SELECT_VALUE(0x140224650, 0x140B2A440, 0x0, 0x0), get_string_width_stub);
				change_language_hook.create(SELECT_VALUE(0x14090FA70, 0x140681A80, 0x0, 0x0), change_language_stub);
			}
		};
	}
}

REGISTER_COMPONENT(renderer::fonts::component)
