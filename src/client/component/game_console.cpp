#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "renderer.hpp"
#include "game_console.hpp"
#include "vars.hpp"

#include <utils/string.hpp>
#include <utils/concurrency.hpp>

namespace game_console
{
	namespace
	{
		vars::var_ptr var_con_input_box_color;
		vars::var_ptr var_con_input_hint_box_color;
		vars::var_ptr var_con_output_bar_color;
		vars::var_ptr var_con_output_slider_color;
		vars::var_ptr var_con_output_window_color;
		vars::var_ptr var_con_input_var_match_color;
		vars::var_ptr var_con_input_var_value_color;
		vars::var_ptr var_con_input_var_inactive_value_color;
		vars::var_ptr var_con_input_cmd_match_color;

		float color_tpp[4] = {0.85f, 0.f, 0.f, 1.f};
		float color_white[4] = {1.f, 1.f, 1.f, 1.f};
		float color_black[4] = {0.f, 0.f, 0.f, 1.f};

		constexpr const auto margin = 4.f;
		constexpr const auto font_height = 12.f;
		constexpr const auto line_height = font_height + 2.f;
		constexpr const auto output_line_count = 49;

		using output_queue = std::deque<std::string>;

		struct
		{
			char input[256];
			int cursor;
			bool toggled;
			bool is_shift_down;
			std::deque<std::string> history;
			std::int32_t history_index = -1;
			bool show_output;
			int display_line_offset;
			int line_count;
			utils::concurrency::container<output_queue, std::recursive_mutex> output{};
		} con{};

		struct match_t
		{
			bool is_var;
			vars::var_ptr var;
			std::string name;
		};

		constexpr const auto max_len = sizeof(con.input) - 1;

		void draw_input_box(game::fox::gr::dg::plugins::Draw2DRenderer* instance, vars::color_t var_color)
		{
			float color[4]{};
			color[0] = var_color.r;
			color[1] = var_color.g;
			color[2] = var_color.b;
			color[3] = var_color.a;

			float dark_color[4]{};
			dark_color[0] = color[0] * 0.5f;
			dark_color[1] = color[1] * 0.5f;
			dark_color[2] = color[2] * 0.5f;
			dark_color[3] = color[3];

			renderer::draw_box(instance, margin, margin, 1280.f - margin * 2.f, line_height, color, dark_color, 1.f);
		}

		float draw_input_branding(game::fox::gr::dg::plugins::Draw2DRenderer* instance)
		{
			const auto width = renderer::draw_text(instance, "tpp-mod > ", font_height, margin + 2.f, margin + 1.f, color_tpp, color_black);
			return margin + 4.f + width;
		}

		void draw_input_text(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float offset)
		{
			renderer::draw_text_with_cursor(instance, con.input, con.cursor, font_height, offset, margin + 1.5f, color_white, color_black, true, 1280.f - offset - margin);
		}

		void find_matches(const std::string& input, std::vector<match_t>& matches)
		{
			if (input.size() < 1)
			{
				return;
			}

			auto lower = utils::string::to_lower(input);
			const auto space_index = input.find(" ");
			const auto exact = space_index != std::string::npos;
			if (exact)
			{
				lower = lower.substr(0, space_index);
			}

			for (auto& var : vars::get_var_list())
			{
				match_t match{};
				match.is_var = true;

				if (exact && var->name == lower)
				{
					match.var = var;
					match.name = var->name;
					matches.emplace_back(match);
					return;
				}
				else if (!exact && var->name.contains(lower))
				{
					match.var = var;
					match.name = var->name;
					matches.emplace_back(match);
				}
			}

			for (const auto& command : command::get_commands())
			{
				match_t match{};
				match.is_var = false;

				if (exact && command.first == lower)
				{
					match.name = command.first;
					matches.emplace_back(match);
					return;
				}
				else if (!exact && command.first.contains(lower))
				{
					match.name = command.first;
					matches.emplace_back(match);
				}
			}
		}

		void draw_hint_box(game::fox::gr::dg::plugins::Draw2DRenderer* instance, int lines, float x_offset, float y_offset, vars::color_t var_color)
		{
			float color[4]{};
			color[0] = var_color.r;
			color[1] = var_color.g;
			color[2] = var_color.b;
			color[3] = var_color.a;

			float dark_color[4]{};
			dark_color[0] = color[0] * 0.5f;
			dark_color[1] = color[1] * 0.5f;
			dark_color[2] = color[2] * 0.5f;
			dark_color[3] = color[3];

			renderer::draw_box(instance, x_offset - 2.f, 
				line_height + margin + 2.f + y_offset, 1280.f - x_offset - 2.f, line_height * lines, color, dark_color, 1.f);
		}

		void draw_hint_text(game::fox::gr::dg::plugins::Draw2DRenderer* instance, int line, const char* text, 
			float* color, float x_offset = 0.f, float y_offset = 0.f)
		{
			const auto y = line_height + margin + 2.f + y_offset + line * line_height + 1.f;
			renderer::draw_text(instance, text, font_height, x_offset, y, color, color_black, true, 200.f, font_height);
		}

		void draw_hint_text(game::fox::gr::dg::plugins::Draw2DRenderer* instance, int line, const char* text, 
			vars::color_t var_color, float x_offset = 0.f, float y_offset = 0.f)
		{
			float color[4]{};
			color[0] = var_color.r;
			color[1] = var_color.g;
			color[2] = var_color.b;
			color[3] = var_color.a;
			draw_hint_text(instance, line, text, color, x_offset, y_offset);
		}

		void draw_hints(game::fox::gr::dg::plugins::Draw2DRenderer* instance, float offset)
		{
			std::vector<match_t> matches;
			find_matches(con.input, matches);

			constexpr const auto spacing = 200.f;

			if (matches.size() > 24)
			{
				draw_hint_box(instance, 1, offset, 0.f, var_con_input_hint_box_color->current.get_color());
				draw_hint_text(instance, 0, utils::string::va("%i matches (too many to show here)", matches.size()),
					var_con_input_var_match_color->current.get_color(), offset);
			}
			else if (matches.size() == 1)
			{
				auto line_count = matches[0].is_var ? 3 : 1;

				draw_hint_box(instance, line_count, offset, 0.f, var_con_input_hint_box_color->current.get_color());
				draw_hint_text(instance, 0, matches[0].name.data(), matches[0].is_var
					? var_con_input_var_match_color->current.get_color()
					: var_con_input_cmd_match_color->current.get_color(),
					offset);

				if (matches[0].is_var)
				{
					const auto current = matches[0].var->current.to_string();
					const auto reset = matches[0].var->reset.to_string();
					const auto& description = matches[0].var->description;
					const auto domain = vars::get_var_domain(matches[0].var);

					draw_hint_text(instance, 0, current.data(), var_con_input_var_match_color->current.get_color(), offset + spacing);
					draw_hint_text(instance, 1, "default", var_con_input_var_inactive_value_color->current.get_color(), offset + 25.f);
					draw_hint_text(instance, 1, reset.data(), var_con_input_var_inactive_value_color->current.get_color(), offset + spacing);
					draw_hint_text(instance, 2, description.data(), color_white, offset);

					draw_hint_box(instance, 1, offset, line_count * line_height + 2.f, var_con_input_hint_box_color->current.get_color());
					draw_hint_text(instance, 0, domain, var_con_input_cmd_match_color->current.get_color(), offset, line_count * line_height + 2.f);
				}
			}
			else if (matches.size() > 1)
			{
				draw_hint_box(instance, static_cast<int>(matches.size()), offset, 0.f, var_con_input_hint_box_color->current.get_color());
				for (auto i = 0u; i < matches.size(); i++)
				{
					draw_hint_text(instance, i, matches[i].name.data(), matches[i].is_var
						? var_con_input_var_match_color->current.get_color()
						: var_con_input_cmd_match_color->current.get_color(),
						offset);

					if (matches[i].is_var)
					{
						const auto current = matches[i].var->current.to_string();
						const auto& description = matches[i].var->description;

						draw_hint_text(instance, i, current.data(), var_con_input_var_match_color->current.get_color(), offset + spacing);
						draw_hint_text(instance, i, description.data(), var_con_input_var_match_color->current.get_color(), offset + spacing * 2.f);
					}
				}
			}
		}

		void draw_output_box(game::fox::gr::dg::plugins::Draw2DRenderer* instance, vars::color_t var_color)
		{
			float color[4]{};
			color[0] = var_color.r;
			color[1] = var_color.g;
			color[2] = var_color.b;
			color[3] = var_color.a;

			float dark_color[4]{};
			dark_color[0] = color[0] * 0.5f;
			dark_color[1] = color[1] * 0.5f;
			dark_color[2] = color[2] * 0.5f;
			dark_color[3] = color[3];

			renderer::draw_box(instance, margin, margin + line_height + 2.f, 
				1280.f - margin * 2.f, 720.f - line_height - 2.f - margin * 2.f, color, dark_color, 1.f);
		}

		void draw_output_scrollbar(game::fox::gr::dg::plugins::Draw2DRenderer* instance, output_queue& output)
		{
			const auto var_color = var_con_output_bar_color->current.get_color();
			const auto var_color_slider = var_con_output_slider_color->current.get_color();

			float color[4]{};
			color[0] = var_color.r;
			color[1] = var_color.g;
			color[2] = var_color.b;
			color[3] = var_color.a;

			float color_dark[4]{};
			color_dark[0] = color[0] * 0.5f;
			color_dark[1] = color[1] * 0.5f;
			color_dark[2] = color[2] * 0.5f;
			color_dark[3] = color[3];

			float slider_color[4]{};
			slider_color[0] = var_color_slider.r;
			slider_color[1] = var_color_slider.g;
			slider_color[2] = var_color_slider.b;
			slider_color[3] = var_color_slider.a;

			float slider_color_dark[4]{};
			slider_color_dark[0] = slider_color[0] * 0.5f;
			slider_color_dark[1] = slider_color[1] * 0.5f;
			slider_color_dark[2] = slider_color[2] * 0.5f;
			slider_color_dark[3] = slider_color[3];

			const auto width = 4.f;
			const auto x = 1280 - margin - 1.f - width;
			auto y = margin + line_height + 3.f;
			const auto height = 720.f - margin * 2.f - line_height - 5.f;

			auto slider_height = height;

			renderer::draw_box(instance, x, y, width, height, color, color_dark, 1.f);

			if (output.size() > output_line_count)
			{
				const auto percentage = static_cast<float>(output_line_count) / output.size();
				slider_height *= percentage;

				const auto remaining = height - slider_height;
				const auto percent_above = static_cast<float>(con.display_line_offset) / (output.size() - output_line_count);

				y += (remaining * percent_above);
			}

			renderer::draw_box(instance, x, y, width, slider_height, slider_color, slider_color_dark, 1.f);
		}

		void draw_output_text(game::fox::gr::dg::plugins::Draw2DRenderer* instance, output_queue& output)
		{
			const auto offset = output.size() >= output_line_count
				? 0.0f
				: (line_height * (output_line_count - output.size()));

			auto y = margin + line_height + 8.f;
			for (auto i = 0; i < std::min(static_cast<int>(output.size()), output_line_count); i++)
			{
				const auto index = i + con.display_line_offset;
				if (index > output.size())
				{
					break;
				}

				renderer::draw_text(instance, output.at(index).data(), font_height, margin + 2.f, y + offset, color_white, color_black, true, 1265.f - margin * 2.f, font_height);
				y += line_height;
			}
		}

		void draw_console(game::fox::gr::dg::plugins::Draw2DRenderer* instance)
		{
			if (!con.toggled)
			{
				return;
			}

			draw_input_box(instance, var_con_input_box_color->current.get_color());

			if (con.show_output)
			{
				draw_output_box(instance, var_con_output_window_color->current.get_color());
				con.output.access([&](output_queue& output)
				{
					draw_output_text(instance, output);
					draw_output_scrollbar(instance, output);
				});
			}

			auto offset = draw_input_branding(instance);
			draw_input_text(instance, offset);
			draw_hints(instance, offset);
		}

		void handle_char(char c)
		{
			if (std::strlen(con.input) >= max_len)
			{
				return;
			}

			std::memmove(con.input + con.cursor + 1, con.input + con.cursor, sizeof(con.input) - con.cursor - 1);
			con.input[con.cursor] = c;
			con.cursor++;

			if (con.cursor == max_len)
			{
				con.input[con.cursor] = 0;
			}
		}

		void handle_backspace()
		{
			if (con.cursor <= 0)
			{
				return;
			}

			std::memmove(con.input + con.cursor - 1, con.input + con.cursor,
				std::strlen(con.input) + 1 - con.cursor);
			con.cursor--;
		}

		void handle_tab()
		{
			std::vector<match_t> matches;
			find_matches(con.input, matches);

			if (matches.empty())
			{
				return;
			}

			const auto& name = matches[0].name;
			if (name.size() >= sizeof(con.input) - 1)
			{
				return;
			}

			std::memcpy(con.input, name.data(), name.size());
			con.cursor = static_cast<int>(name.size());
			con.input[con.cursor++] = ' ';
		}

		void clear_input()
		{
			con.cursor = 0;
			std::memset(con.input, 0, sizeof(con.input));
		}

		void handle_return()
		{
			command::execute(con.input);
			clear_input();
		}

		void handle_paste()
		{
			const auto clipboard = utils::string::get_clipboard_data();

			for (auto c : clipboard)
			{
				if (con.cursor >= sizeof(con.input))
				{
					return;
				}

				c = utils::string::normalize_ascii_extended(c);
				if (utils::string::is_char_text(c))
				{
					handle_char(c);
				}
			}
		}

		void move_cursor(bool right)
		{
			if (right && con.input[con.cursor] != '\0')
			{
				con.cursor++;
			}
			else if (!right && con.cursor > 0)
			{
				con.cursor--;
			}
		}

		void handle_delete()
		{
			auto is_first = true;
			while (con.cursor > 0)
			{
				const auto is_letter = isalnum(con.input[con.cursor - 1]);

				if (is_letter || is_first)
				{
					handle_backspace();
				}

				if (!is_letter)
				{
					return;
				}

				is_first = false;
			}
		}

		void handle_up()
		{
			if (++con.history_index >= con.history.size())
			{
				con.history_index = static_cast<int>(con.history.size()) - 1;
			}

			std::memset(con.input, 0, sizeof(con.input));
			con.cursor = 0;

			if (con.history_index != -1)
			{
				strncpy_s(con.input, con.history.at(con.history_index).data(), sizeof(con.input));
				con.cursor = static_cast<int>(std::strlen(con.input));
			}
		}

		void handle_down()
		{
			if (--con.history_index < -1)
			{
				con.history_index = -1;
			}

			std::memset(con.input, 0, sizeof(con.input));
			con.cursor = 0;

			if (con.history_index != -1)
			{
				strncpy_s(con.input, con.history.at(con.history_index).data(), sizeof(con.input));
				con.cursor = static_cast<int>(strlen(con.input));
			}
		}

		void update_history()
		{
			if (con.history_index != -1)
			{
				const auto itr = con.history.begin() + con.history_index;

				if (*itr == con.input)
				{
					con.history.erase(con.history.begin() + con.history_index);
				}
			}

			if (con.input[0])
			{
				con.history.push_front(con.input);
			}

			if (con.history.size() > 10)
			{
				con.history.erase(con.history.begin() + 10);
			}

			con.history_index = -1;
		}

		void clear_console()
		{
			clear_input();
			con.line_count = 0;
			con.output.access([](output_queue& output)
			{
				output.clear();
			});
			con.history_index = -1;
			con.history.clear();
		}

		void exit_console()
		{
			clear_input();
			con.toggled = false;
			con.show_output = false;
		}
	}

	bool handle_key(const int key, const bool is_down, const bool is_console_bind)
	{
		if (!con.toggled || (!con.is_shift_down && is_console_bind))
		{
			return false;
		}

		if (key == VK_SHIFT)
		{
			con.is_shift_down = is_down;
		}

		if (!is_down)
		{
			return true;
		}

		switch (key)
		{
		case VK_UP:
			handle_up();
			break;
		case VK_DOWN:
			handle_down();
			break;
		case VK_LEFT:
			move_cursor(false);
			break;
		case VK_RIGHT:
			move_cursor(true);
			break;
		case VK_TAB:
			handle_tab();
			break;
		case VK_ESCAPE:
			exit_console();
			break;
		case VK_BACK:
			handle_backspace();
			break;
		case VK_RETURN:
			update_history();
			handle_return();
			break;
		}

		return true;
	}

	bool handle_char(const int key, const bool is_down)
	{
		if (!con.toggled)
		{
			return false;
		}

		if (!is_down || key >= 0xFF)
		{
			return true;
		}

		if (con.is_shift_down && key == '`' || key == '|')
		{
			con.show_output = !con.show_output;
			return true;
		}

		switch (key)
		{
		case 0x1:
			clear_input();
			break;
		case 0x16:
			handle_paste();
			break;
		case 0x7F:
			handle_delete();
			break;
		default:
		{
			const auto c = utils::string::normalize_ascii_extended(static_cast<char>(key));
			if (utils::string::is_char_text(c))
			{
				handle_char(c);
			}
			break;
		}
		}

		return true;
	}

	bool handle_mousewheel(const bool down)
	{
		if (!con.toggled)
		{
			return false;
		}

		con.output.access([&](output_queue& output)
		{
			if (!down)
			{
				if (output.size() > output_line_count && con.display_line_offset > 0)
				{
					con.display_line_offset--;
				}
			}
			else
			{
				if (output.size() > output_line_count && con.display_line_offset < (output.size() - output_line_count))
				{
					con.display_line_offset++;
				}
			}
		});

		return true;
	}

	void print(const std::string& data)
	{
		con.output.access([&](output_queue& output)
		{
			if (output_line_count > 0 && con.display_line_offset == (output.size() - output_line_count))
			{
				con.display_line_offset++;
			}

			output.emplace_back(data);
			if (output.size() > 512)
			{
				output.pop_front();
			}
		});
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			command::add("clear", clear_console);

			command::add("toggleconsole", []()
			{
				clear_input();
				con.toggled = !con.toggled;
				con.show_output = false;
			});

			var_con_input_box_color = vars::register_color(
				"con_input_box_color",
				{0.2f, 0.2f, 0.2f, 0.9f},
				vars::var_flag_saved,
				"color of console input box"
			);

			var_con_input_hint_box_color = vars::register_color(
				"con_input_hint_box_color",
				{0.3f, 0.3f, 0.3f, 1.0f},
				vars::var_flag_saved,
				"color of console input hint box"
			);

			var_con_output_bar_color = vars::register_color(
				"con_output_bar_color",
				{0.5f, 0.5f, 0.5f, 0.6f},
				vars::var_flag_saved,
				"color of console output bar"
			);

			var_con_output_slider_color = vars::register_color(
				"con_output_slider_color",
				{color_tpp[0], color_tpp[1], color_tpp[2], color_tpp[3]},
				vars::var_flag_saved,
				"color of console output slider"
			);

			var_con_output_window_color = vars::register_color(
				"con_output_window_color",
				{0.25f, 0.25f, 0.25f, 0.85f},
				vars::var_flag_saved,
				"color of console output window"
			);

			var_con_input_var_match_color = vars::register_color(
				"con_input_dvar_match_color",
				{1.0f, 1.0f, 0.8f, 1.0f},
				vars::var_flag_saved,
				"color of console matched dvar"
			);

			var_con_input_var_value_color = vars::register_color(
				"con_input_dvar_value_color",
				{1.0f, 1.0f, 0.8f, 1.0f},
				vars::var_flag_saved,
				"color of console matched dvar value"
			);

			var_con_input_var_inactive_value_color = vars::register_color(
				"con_input_dvar_inactive_value_color",
				{0.8f, 0.8f, 0.8f, 1.0f},
				vars::var_flag_saved,
				"color of console inactive dvar value"
			);

			var_con_input_cmd_match_color = vars::register_color(
				"con_input_cmd_match_color",
				{0.80f, 0.80f, 1.0f, 1.0f},
				vars::var_flag_saved,
				"color of console matched command"
			);
		}

		void start() override
		{
			renderer::on_frame(draw_console);
		}
	};
}

REGISTER_COMPONENT(game_console::component)
