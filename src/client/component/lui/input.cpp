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
		struct mouse_state_t
		{
			float last_x;
			float last_y;
			float delta_x;
			float delta_y;
			bool keys[6];
			bool prev_keys[6];
		};

		struct key_event_t
		{
			int key;
			bool is_down;
			bool is_char;
			bool is_mousewheel;
		};

		using key_event_queue_t = std::vector<key_event_t>;

		utils::concurrency::container<key_event_queue_t> key_event_queue;
		utils::concurrency::container<mouse_state_t> mouse_state;
		bool needs_key_catcher;
		std::atomic_bool key_catcher_enabled;

		void show_cursor()
		{
			ShowCursor(TRUE);
		}

		void update_input_mouse()
		{
			auto& root = get_root_element();
			const auto state = mouse_state.access<mouse_state_t>([](mouse_state_t& state)
			{
				auto copy = state;
				state.delta_x = 0.f;
				state.delta_y = 0.f;
				std::memcpy(state.prev_keys, state.keys, sizeof(state.keys));
				return copy;
			});

			{
				mouse_move_params_t params{};

				params.x = state.last_x;
				params.y = state.last_y;
				params.delta_x = state.delta_x;
				params.delta_y = state.delta_y;

				if (params.delta_x != 0 || params.delta_y != 0)
				{
					root->handle_mouse_move(params);
				}
			}

			for (auto i = 0; i < 6; i++)
			{
				if (state.prev_keys[i] != state.keys[i])
				{
					mouse_button_params_t params{};
					params.button = i;
					params.is_down = state.keys[i];
					root->handle_mouse_button(params);
				}
			}
		}

		void update_input_keys(const key_event_queue_t& event_queue)
		{
			auto& root = get_root_element();
			for (const auto& key_event : event_queue)
			{
				event_t event{};
				event.target = root;
				event.immediate = true;
				event.dispatch_children = true;

				if (key_event.is_char)
				{
					event.name = "char";
					event.params.set("key", key_event.key);
				}
				else if (key_event.is_mousewheel)
				{
					event.name = "mousewheel";
					event.params.set("down", key_event.is_down);
				}
				else
				{
					event.name = key_event.is_down ? "keydown" : "keyup";
					event.params.set("key", key_event.key);
				}

				root->dispatch_event(event);
			}
		}
	}

	void update()
	{
		const auto queue = key_event_queue.access<key_event_queue_t>([&](key_event_queue_t& queue)
		{
			auto copy = queue;
			queue.clear();
			return copy;
		});

		update_input_mouse();
		update_input_keys(queue);

		needs_key_catcher = false;
	}

	void post_update()
	{
		if (!needs_key_catcher)
		{
			mouse_state.access([](mouse_state_t& state)
			{
				std::memset(state.prev_keys, 0, sizeof(state.prev_keys));
				std::memset(state.keys, 0, sizeof(state.keys));
			});
		}

		key_catcher_enabled = needs_key_catcher;
	}

	void set_key_catcher()
	{
		needs_key_catcher = true;
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

		mouse_state.access([&](mouse_state_t& state)
		{
			state.delta_x += (x - state.last_x);
			state.delta_y += (y - state.last_y);

			state.last_x = x;
			state.last_y = y;
		});
	}

	void handle_mouse_button(const int button, const bool is_down)
	{
		mouse_state.access([&](mouse_state_t& state)
		{
			state.keys[button] = is_down;
		});
	}

	bool handle_mouse_wheel(const bool down)
	{
		if (!key_catcher_enabled)
		{
			return false;
		}

		key_event_t key_event{};
		key_event.key = 0;
		key_event.is_down = down;
		key_event.is_mousewheel = true;

		key_event_queue.access([&](key_event_queue_t& queue)
		{
			queue.emplace_back(key_event);
		});

		return true;
	}

	bool handle_key(const int key, const bool is_down, const bool is_game_console_bind)
	{
		if (!key_catcher_enabled || is_game_console_bind)
		{
			return false;
		}

		key_event_t key_event{};
		key_event.key = key;
		key_event.is_down = is_down;

		key_event_queue.access([&](key_event_queue_t& queue)
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

		key_event_queue.access([&](key_event_queue_t& queue)
		{
			queue.emplace_back(key_event);
		});

		return true;
	}

	bool is_key_catcher_enabled()
	{
		return key_catcher_enabled;
	}
}
