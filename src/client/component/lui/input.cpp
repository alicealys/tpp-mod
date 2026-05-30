#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "input.hpp"
#include "main.hpp"

#include "../scheduler.hpp"
#include "../console.hpp"

#include <utils/concurrency.hpp>

namespace lui::input
{
	namespace
	{
		struct
		{
			std::mutex mutex;
			float last_x;
			float last_y;
			float delta_x;
			float delta_y;
			bool keys[6];
			bool prev_keys[6];
		} mouse_state{};

		struct key_event_t
		{
			int key;
			bool is_down;
			bool is_char;
			bool is_mousewheel;
		};

		using key_event_queue_t = std::vector<key_event_t>;

		struct
		{
			utils::concurrency::container<key_event_queue_t> key_event_queue;
			bool needs_key_catcher;
			std::atomic_bool key_catcher_enabled;
		} state{};

		void show_cursor()
		{
			ShowCursor(TRUE);
		}

		void update_input_mouse()
		{
			std::lock_guard _0(mouse_state.mutex);

			{
				mouse_move_params_t params{};

				params.x = mouse_state.last_x;
				params.y = mouse_state.last_y;
				params.delta_x = mouse_state.delta_x;
				params.delta_y = mouse_state.delta_y;

				mouse_state.delta_x = 0;
				mouse_state.delta_y = 0;

				if (params.delta_x != 0 || params.delta_y != 0)
				{
					get_root_element()->handle_mouse_move(params);
				}
			}

			for (auto i = 0; i < 6; i++)
			{
				if (mouse_state.prev_keys[i] != mouse_state.keys[i])
				{
					mouse_button_params_t params{};
					params.button = i;
					params.is_down = mouse_state.keys[i];
					get_root_element()->handle_mouse_button(params);
				}

				mouse_state.prev_keys[i] = mouse_state.keys[i];
			}
		}

		void update_input_keys(const key_event_queue_t& event_queue)
		{
			for (const auto& key_event : event_queue)
			{
				event_t event{};
				event.target = get_root_element();
				event.immediate = true;
				event.dispatch_children = true;

				if (key_event.is_char)
				{
					event.name = "char";
					event.set("key", key_event.key);
				}
				else if (key_event.is_mousewheel)
				{
					event.name = "mousewheel";
					event.set("down", key_event.is_down);
				}
				else
				{
					event.name = key_event.is_down ? "keydown" : "keyup";
					event.set("key", key_event.key);
				}

				get_root_element()->dispatch_event(event);
			}
		}
	}

	void update()
	{
		const auto queue = state.key_event_queue.access<key_event_queue_t>([&](key_event_queue_t& queue)
		{
			auto copy = queue;
			queue.clear();
			return copy;
		});

		update_input_mouse();
		update_input_keys(queue);

		state.needs_key_catcher = false;
	}

	void post_update()
	{
		state.key_catcher_enabled = state.needs_key_catcher;

		if (!state.key_catcher_enabled)
		{
			std::memset(mouse_state.prev_keys, 0, sizeof(mouse_state.prev_keys));
			std::memset(mouse_state.keys, 0, sizeof(mouse_state.keys));
		}
	}

	void set_key_catcher()
	{
		state.needs_key_catcher = true;
	}

	void handle_mousemove(HWND hwnd)
	{
		show_cursor();

		RECT rect{};
		POINT point{};
		GetClientRect(hwnd, &rect);

		if (!GetCursorPos(&point) || !ScreenToClient(hwnd, &point))
		{
			return;
		}

		const auto width = rect.right - rect.left;
		const auto height = rect.bottom - rect.top;

		const auto x = static_cast<float>(point.x) / static_cast<float>(width) * 1280.f;
		const auto y = static_cast<float>(point.y) / static_cast<float>(height) * 720.f;

		std::lock_guard _0(mouse_state.mutex);

		mouse_state.delta_x += (x - mouse_state.last_x);
		mouse_state.delta_y += (y - mouse_state.last_y);

		mouse_state.last_x = x;
		mouse_state.last_y = y;
	}

	void handle_mouse_button(const int button, const bool is_down)
	{
		mouse_state.keys[button] = is_down;
	}

	bool handle_mouse_wheel(const bool down)
	{
		if (!state.key_catcher_enabled)
		{
			return false;
		}

		key_event_t key_event{};
		key_event.key = 0;
		key_event.is_down = down;
		key_event.is_mousewheel = true;

		state.key_event_queue.access([&](key_event_queue_t& queue)
		{
			queue.emplace_back(key_event);
		});

		return true;
	}

	bool handle_key(const int key, const bool is_down, const bool is_game_console_bind)
	{
		if (!state.key_catcher_enabled || is_game_console_bind)
		{
			return false;
		}

		key_event_t key_event{};
		key_event.key = key;
		key_event.is_down = is_down;

		state.key_event_queue.access([&](key_event_queue_t& queue)
		{
			queue.emplace_back(key_event);
		});

		return true;
	}

	bool handle_char(const int key, const bool is_down, const bool is_game_console_bind)
	{
		if (!is_down)
		{
			return false;
		}

		key_event_t key_event{};
		key_event.key = key;
		key_event.is_down = is_down;
		key_event.is_char = true;

		state.key_event_queue.access([&](key_event_queue_t& queue)
		{
			queue.emplace_back(key_event);
		});

		return true;
	}

	bool is_key_catcher_enabled()
	{
		return state.key_catcher_enabled;
	}
}
