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
		constexpr const auto draw_cmd_buffer_size = 0x10000;

		struct
		{
			std::mutex mutex;
			char* cmd_buffer;
			char* cmd_buffer_pos;
			char* cmd_buffer_end;
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
			draw_list.cmd_buffer_pos += sizeof(T);
			return cmd;
		}

		void render_ui(game::fox::gr::dg::plugins::Draw2DRenderer* renderer)
		{
			std::lock_guard _0(draw_list.mutex);
			auto base_cmd = reinterpret_cast<draw_command*>(draw_list.cmd_buffer);
			while (base_cmd->type != CMD_NONE)
			{
				switch (base_cmd->type)
				{
				case CMD_DRAW_BOX:
				{
					const auto cmd = reinterpret_cast<draw_box_command*>(base_cmd);
					::renderer::draw_box(renderer, cmd->x, cmd->y, cmd->width, cmd->height, cmd->color);
					break;
				}
				case CMD_DRAW_TEXT:
				{
					const auto cmd = reinterpret_cast<draw_text_command*>(base_cmd);
					::renderer::draw_text(renderer, cmd->text, cmd->height, cmd->x, cmd->y, cmd->color,
						cmd->outline_color, cmd->formatted, cmd->display_width, cmd->display_width, 0.f, 0.f, cmd->use_word_wrapping);
					break;
				}
				}

				base_cmd->type = CMD_NONE;
				base_cmd = reinterpret_cast<draw_command*>(reinterpret_cast<size_t>(base_cmd) + base_cmd->size);
			}

			draw_list.cmd_buffer_pos = draw_list.cmd_buffer;
		}
	}

	void add_draw_box(float x, float y, float width, float height, float* color)
	{
		std::lock_guard _0(draw_list.mutex);

		auto cmd = allocate_draw_command<draw_box_command>();
		cmd->type = CMD_DRAW_BOX;
		cmd->x = x;
		cmd->y = y;
		cmd->width = width;
		cmd->height = height;
		std::memcpy(cmd->color, color, sizeof(float[4]));
	}

	void add_draw_text(const char* text, float height, float x, float y, float* color, float* outline_color, bool formatted, float display_width, float display_height, bool use_word_wrapping)
	{
		std::lock_guard _0(draw_list.mutex);

		auto cmd = allocate_draw_command<draw_text_command>();
		cmd->type = CMD_DRAW_TEXT;
		strcpy_s(cmd->text, sizeof(cmd->text), text);
		cmd->height = height;
		cmd->x = x;
		cmd->y = y;
		cmd->formatted = formatted;
		cmd->display_width = display_width;
		cmd->display_height = display_height;
		cmd->use_word_wrapping = use_word_wrapping;
		std::memcpy(cmd->color, color, sizeof(float[4]));
		std::memcpy(cmd->outline_color, outline_color, sizeof(float[4]));
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			draw_list.cmd_buffer = utils::memory::allocate_array<char>(draw_cmd_buffer_size);
			draw_list.cmd_buffer_pos = draw_list.cmd_buffer;
			draw_list.cmd_buffer_end = draw_list.cmd_buffer + draw_cmd_buffer_size;

			::renderer::on_frame(render_ui);
		}
	};
}

#ifdef DEBUG
REGISTER_COMPONENT(lui::renderer::component)
#endif
