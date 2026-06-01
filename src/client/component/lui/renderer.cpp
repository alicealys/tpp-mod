#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "renderer.hpp"
#include "../command.hpp"
#include "../renderer.hpp"

#include <utils/memory.hpp>

namespace lui::renderer
{
	namespace
	{
		constexpr const auto draw_cmd_buffer_size = 0x100000;

		struct
		{
			std::mutex mutex;
			char* cmd_buffer;
			char* cmd_buffer_pos;
			char* cmd_buffer_end;
			std::size_t count;
		} draw_list{};

		template <typename T>
		T* allocate_draw_command()
		{
			if (draw_list.cmd_buffer_pos + sizeof(T) > draw_list.cmd_buffer_end)
			{
				return nullptr;
			}

			const auto cmd = reinterpret_cast<T*>(draw_list.cmd_buffer_pos);
			std::memset(cmd, 0, sizeof(T));
			cmd->size = sizeof(T);
			draw_list.count++;
			draw_list.cmd_buffer_pos += sizeof(T);
			return cmd;
		}

		void render_ui(game::fox::gr::dg::plugins::Draw2DRenderer* renderer)
		{
			std::lock_guard _0(draw_list.mutex);
			auto base_cmd = reinterpret_cast<draw_command*>(draw_list.cmd_buffer);
			auto i = 0;
			while (base_cmd->type != CMD_NONE && i < draw_list.count && reinterpret_cast<char*>(base_cmd) < draw_list.cmd_buffer_end)
			{
				switch (base_cmd->type)
				{
				case CMD_DRAW_BOX:
				{
					const auto cmd = reinterpret_cast<draw_box_command*>(base_cmd);
					::renderer::draw_material(renderer, cmd->material, cmd->x, cmd->y, cmd->width, cmd->height, cmd->color, cmd->rotation);
					break;
				}
				case CMD_DRAW_TEXT:
				{
					const auto cmd = reinterpret_cast<draw_text_command*>(base_cmd);
					if (cmd->artist_font)
					{
						::renderer::draw_text_artist(renderer, cmd->text, cmd->height, cmd->x, cmd->y, cmd->color,
							cmd->formatted, cmd->display_width, cmd->display_height, 0.f, 0.f, cmd->use_word_wrapping);
					}
					else
					{
						::renderer::draw_text(renderer, cmd->text, cmd->height, cmd->x, cmd->y, cmd->color,
							cmd->outline_color, cmd->formatted, cmd->display_width, cmd->display_height, 0.f, 0.f, cmd->use_word_wrapping, -1, cmd->rotation);
					}

					break;
				}
				case CMD_DRAW_TEXT_CURSOR:
				{
					const auto cmd = reinterpret_cast<draw_text_cursor_command*>(base_cmd);
					::renderer::draw_text_with_cursor(renderer, cmd->text, cmd->cursor, cmd->height, cmd->x, cmd->y, cmd->color, cmd->outline_color, 
						cmd->formatted, cmd->display_width, cmd->rotation);
				}
				}

				++i;
				base_cmd->type = CMD_NONE;
				base_cmd = reinterpret_cast<draw_command*>(reinterpret_cast<size_t>(base_cmd) + base_cmd->size);
			}

			draw_list.count = 0;
			draw_list.cmd_buffer_pos = draw_list.cmd_buffer;
		}

		bool allocate_command_buffer()
		{
			if (!VirtualAlloc(NULL, draw_cmd_buffer_size, MEM_RESERVE, PAGE_READWRITE))
			{
				return false;
			}

			draw_list.cmd_buffer = reinterpret_cast<char*>(VirtualAlloc(NULL, draw_cmd_buffer_size, MEM_COMMIT, PAGE_READWRITE));
			draw_list.cmd_buffer_pos = draw_list.cmd_buffer;
			draw_list.cmd_buffer_end = draw_list.cmd_buffer + draw_cmd_buffer_size;
			return true;
		}

		void free_command_buffer()
		{
			if (draw_list.cmd_buffer != nullptr)
			{
				VirtualFree(draw_list.cmd_buffer, 0ull, MEM_RELEASE);
				draw_list.cmd_buffer = nullptr;
			}
		}
	}

	void add_draw_material(const std::uint32_t material, float x, float y, float width, float height, float* color, float rotation)
	{
		std::lock_guard _0(draw_list.mutex);

		auto cmd = allocate_draw_command<draw_box_command>();
		cmd->type = CMD_DRAW_BOX;
		cmd->x = x;
		cmd->y = y;
		cmd->width = width;
		cmd->height = height;
		cmd->rotation = rotation;
		cmd->material = material;
		std::memcpy(cmd->color, color, sizeof(float[4]));
	}

	void add_draw_text(const char* text, float height, float x, float y, float* color, float* outline_color, bool formatted, 
		float display_width, float display_height, bool use_word_wrapping, float rotation, bool artist_font)
	{
		std::lock_guard _0(draw_list.mutex);

		auto cmd = allocate_draw_command<draw_text_command>();
		cmd->type = CMD_DRAW_TEXT;
		strncpy_s(cmd->text, sizeof(cmd->text), text, _TRUNCATE);
		cmd->height = height;
		cmd->x = x;
		cmd->y = y;
		cmd->formatted = formatted;
		cmd->display_width = display_width;
		cmd->display_height = display_height;
		cmd->use_word_wrapping = use_word_wrapping;
		cmd->rotation = rotation;
		cmd->artist_font = artist_font;

		if (color != nullptr)
		{
			std::memcpy(cmd->color, color, sizeof(float[4]));
		}

		if (outline_color != nullptr)
		{
			std::memcpy(cmd->outline_color, outline_color, sizeof(float[4]));
		}
	}

	void add_draw_text_with_cursor(const char* text, float height, float x, float y, float* color, float* outline_color, bool formatted,
		float display_width, float rotation, int cursor)
	{
		std::lock_guard _0(draw_list.mutex);

		auto cmd = allocate_draw_command<draw_text_cursor_command>();
		cmd->type = CMD_DRAW_TEXT_CURSOR;
		strncpy_s(cmd->text, sizeof(cmd->text), text, _TRUNCATE);
		cmd->height = height;
		cmd->x = x;
		cmd->y = y;
		cmd->formatted = formatted;
		cmd->display_width = display_width;
		cmd->rotation = rotation;
		cmd->cursor = cursor;

		if (color != nullptr)
		{
			std::memcpy(cmd->color, color, sizeof(float[4]));
		}

		if (outline_color != nullptr)
		{
			std::memcpy(cmd->outline_color, outline_color, sizeof(float[4]));
		}
	}

	void load()
	{
		if (!allocate_command_buffer())
		{
			return;
		}

		::renderer::on_frame(render_ui);
	}
	
	void end()
	{
		free_command_buffer();
	}
}
