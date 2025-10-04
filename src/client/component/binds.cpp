#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "console.hpp"
#include "command.hpp"
#include "filesystem.hpp"

#include "text_chat/input.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/properties.hpp>
#include <utils/io.hpp>

namespace binds
{
	namespace
	{
		utils::hook::detour wnd_proc_hook;

		enum bind_mode_t
		{
			none,
			command,
			remap,
		};

		struct bind_t
		{
			std::string command;
			int mapped_key;
			bind_mode_t mode;
		};

		std::map<int, bind_t> binds;
		std::atomic_bool block_input = false;

		std::unordered_map<std::string, int> key_map =
		{
			{"SPACE", VK_SPACE},
			{"TAB", VK_TAB},
			{"ENTER", VK_ACCEPT},
			{"ESCAPE", VK_ESCAPE},
			{"SPACE", VK_SPACE},
			{"BACKSPACE", VK_BACK},
			{"UPARROW", VK_UP},
			{"DOWNARROW", VK_DOWN},
			{"LEFTARROW", VK_LEFT},
			{"RIGHTARROW", VK_RIGHT},
			{"ALT", VK_MENU},
			{"CTRL", VK_CONTROL},
			{"SHIFT", VK_SHIFT},
			{"CAPSLOCK", VK_CAPITAL},
			{"F1", VK_F1},
			{"F2", VK_F2},
			{"F3", VK_F3},
			{"F4", VK_F4},
			{"F5", VK_F5},
			{"F6", VK_F6},
			{"F7", VK_F7},
			{"F8", VK_F8},
			{"F9", VK_F9},
			{"F10", VK_F10},
			{"F11", VK_F11},
			{"F12", VK_F12},
			{"INS", VK_INSERT},
			{"DEL", VK_DELETE},
			{"PGDN", VK_NEXT},
			{"PGUP", VK_PRIOR},
			{"HOME", VK_HOME},
			{"END", VK_END},
			{"MOUSE1", VK_LBUTTON},
			{"MOUSE2", VK_RBUTTON},
			{"MOUSE3", VK_MBUTTON},
			{"MOUSE4", VK_XBUTTON1},
			{"MOUSE5", VK_XBUTTON2},
		};

		int find_key(const std::string& key)
		{
			if (key.size() == 0)
			{
				return -1;
			}

			const auto upper = utils::string::to_upper(key);

			if (upper.size() == 1)
			{
				return upper[0];
			}
			else if (upper.size() > 1)
			{
				const auto iter = key_map.find(upper);
				if (iter == key_map.end())
				{
					return -1;
				}

				return iter->second;
			}

			return -1;
		}

		std::string find_key_rev(const int key)
		{
			for (const auto& [name, value] : key_map)
			{
				if (value == key)
				{
					return name;
				}
			}

			return utils::string::va("%c", key);
		}

		void execute_remap(int key, int mapped_key, bool is_down, bool is_up)
		{
			if (mapped_key >= VK_LBUTTON && mapped_key <= VK_XBUTTON2)
			{
				// todo
			}
			else
			{
				game::fox::RawKeyboardData data{};
				data.isUp = is_up;
				data.key = game::fox::g_vkKeyTable[mapped_key];
				data.keyAscii = -1;
				data.keyUnicode = 0;
				game::fox::KeyboardListener_::SetRawKeyData(&data);
			}
		}

		bool handle_bind(int key, bool is_down, bool is_up)
		{
			const auto iter = binds.find(key);
			if (iter == binds.end())
			{
				return false;
			}

			if (is_down && iter->second.mode == command)
			{
				command::execute(iter->second.command);
			}
			else if (iter->second.mode == remap)
			{
				execute_remap(key, iter->second.mapped_key, is_down, is_up);
			}

			return true;
		}

		bool handle_input_keyboard(RAWINPUT* raw_input)
		{
			BYTE key_state[256]{};
			WORD key_char[8]{};

			int key = raw_input->data.keyboard.VKey;

			if (!GetKeyboardState(key_state))
			{
				return false;
			}

			if (ToAscii(raw_input->data.keyboard.VKey, raw_input->data.keyboard.MakeCode, key_state, key_char, 0) > 0)
			{
				key = toupper(key_char[0]);
			}

			return handle_bind(key, raw_input->data.keyboard.Flags == RI_KEY_MAKE, raw_input->data.keyboard.Flags == RI_KEY_BREAK);
		}

		bool handle_input_mouse(RAWINPUT* raw_input)
		{
			bool has_bind{};

			const auto do_button = [&](int key, int flag_down, int flag_up)
			{
				const auto is_down = (raw_input->data.mouse.usButtonFlags & flag_down) != 0;
				const auto is_up = (raw_input->data.mouse.usButtonFlags & flag_up) != 0;

				if (!is_down && !is_up)
				{
					return;
				}

				if (handle_bind(key, is_down, is_up))
				{
					has_bind = true;
				}
			};

			do_button(VK_LBUTTON, RI_MOUSE_BUTTON_1_DOWN, RI_MOUSE_BUTTON_1_UP);
			do_button(VK_RBUTTON, RI_MOUSE_BUTTON_2_DOWN, RI_MOUSE_BUTTON_2_UP);
			do_button(VK_MBUTTON, RI_MOUSE_BUTTON_3_DOWN, RI_MOUSE_BUTTON_3_UP);
			do_button(VK_XBUTTON1, RI_MOUSE_BUTTON_4_DOWN, RI_MOUSE_BUTTON_4_UP);
			do_button(VK_XBUTTON2, RI_MOUSE_BUTTON_5_DOWN, RI_MOUSE_BUTTON_5_UP);

			return has_bind;
		}

		bool get_raw_input(LPARAM l_param, RAWINPUT* raw_input)
		{
			unsigned int dw_size{};
			GetRawInputData(reinterpret_cast<HRAWINPUT>(l_param), RID_INPUT, NULL, &dw_size, sizeof(RAWINPUTHEADER));

			if (dw_size > sizeof(RAWINPUT))
			{
				return false;
			}

			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(l_param), RID_INPUT, raw_input, &dw_size, sizeof(RAWINPUTHEADER)) != dw_size)
			{
				return false;
			}

			return true;
		}

		bool handle_input(LPARAM l_param)
		{
			RAWINPUT raw_input{};
			if (!get_raw_input(l_param, &raw_input))
			{
				return false;
			}

			switch (raw_input.header.dwType)
			{
			case RIM_TYPEKEYBOARD:
				return handle_input_keyboard(&raw_input);
			case RIM_TYPEMOUSE:
				return handle_input_mouse(&raw_input);
			}

			return false;
		}

		LRESULT wnd_proc_stub(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
		{
			text_chat::input::handle_typing(msg, w_param, l_param);

			if (msg == WM_INPUT && (text_chat::input::is_input_blocked() || handle_input(l_param)))
			{
				return DefWindowProc(hwnd, msg, w_param, l_param);
			}

			return wnd_proc_hook.invoke<LRESULT>(hwnd, msg, w_param, l_param);
		}

		std::string get_config_file_path()
		{
			static const auto file = SELECT_VALUE("keys_tpp.cfg", "keys_mgo.cfg");
			return (utils::properties::get_appdata_path() / file).generic_string();
		}

		void write_binds()
		{
			const auto path = get_config_file_path();
			std::string buffer;

			buffer.append("unbindall\r\n");

			for (const auto& [key, bind] : binds)
			{
				if (bind.mode == command)
				{
					const auto key_name = find_key_rev(key);
					buffer.append(utils::string::va("bind %s \"%s\"\r\n", key_name.data(), bind.command.data()));
				}
			}

			for (const auto& [key, bind] : binds)
			{
				if (bind.mode == remap)
				{
					const auto source_key_name = find_key_rev(key);
					const auto target_key_name = find_key_rev(bind.mapped_key);
					buffer.append(utils::string::va("remap %s %s\r\n", source_key_name.data(), target_key_name.data()));
				}
			}

			utils::io::write_file(path, buffer);
		}

		void bind_key(const std::string& key, const std::string& cmd)
		{
			const auto key_code = find_key(key);
			if (key_code == -1)
			{
				return;
			}

			bind_t bind{};
			bind.mode = command;
			bind.command = cmd;

			binds[key_code] = bind;
			write_binds();
		}

		void remap_key(const std::string& source, const std::string& target)
		{
			const auto source_key = find_key(source);
			const auto target_key = find_key(target);
			if (source_key == -1 || target_key == -1)
			{
				return;
			}

			bind_t bind{};
			bind.mode = remap;
			bind.mapped_key = target_key;

			binds[source_key] = bind;
			write_binds();
		}

		void unbind_key(const std::string& key)
		{
			const auto key_code = find_key(key);
			if (key_code == -1)
			{
				return;
			}

			binds.erase(key_code);
			write_binds();
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			filesystem::register_resource_file("keys_tpp.cfg", RESOURCE_KEYS_TPP);
			filesystem::register_resource_file("keys_mgo.cfg", RESOURCE_KEYS_MGO);

			command::add("unbindall", [](const command::params& params)
			{
				binds.clear();
				write_binds();
			});

			command::add("bind", [](const command::params& params)
			{
				if (params.size() < 3)
				{
					return;
				}

				const auto key = params.get(1);
				const auto cmd = params.get(2);
				bind_key(key, cmd);
			});

			command::add("remap", [](const command::params& params)
			{
				if (params.size() < 3)
				{
					return;
				}

				const auto key = params.get(1);
				const auto key_2 = params.get(2);
				remap_key(key, key_2);
			});

			command::add("unbind", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto key = params.get(1);
				unbind_key(key);
			});
		}

		void start() override
		{
			wnd_proc_hook.create(SELECT_VALUE(0x14006FF50, 0x140070FC0), wnd_proc_stub);

			write_binds();
		}
	};
}

REGISTER_COMPONENT(binds::component)
