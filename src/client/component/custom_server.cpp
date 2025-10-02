#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "scheduler.hpp"
#include "console.hpp"
#include "vars.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>
#include <utils/flags.hpp>
#include <utils/io.hpp>
#include <utils/cryptography.hpp>

namespace custom_server
{
	namespace
	{
		char custom_url[0x2000]{};

		utils::hook::detour file_read_hook;
		utils::hook::detour file_write_hook;

		std::unordered_set<std::string> file_names =
		{
			{"TPP_GAME_DATA"},
			{"TPP_GAME_DATA0"},
			{"TPP_GAME_DATA1"},
		};

		vars::var_ptr var_custom_server_tpp;
		vars::var_ptr var_custom_server_mgo;

		struct steam_storage;

		struct steam_storage_vftbl
		{
			bool (__fastcall* file_write)(steam_storage* inst, const char* name, const char* buffer, int size);
			int	(__fastcall* file_read)(steam_storage* inst, const char* name, char* buffer, int size);
		};

		struct steam_storage
		{
			steam_storage_vftbl* vftbl;
		};

		std::string get_custom_server_data_folder()
		{
			const auto url_hash = utils::cryptography::sha1::compute(custom_url, true).substr(0, 8);
			const auto folder = std::format("tpp-mod\\steam_storage\\server-{}", url_hash);
			return folder;
		}

		std::string get_custom_server_data_file_path(const std::string& file_name)
		{
			const auto folder = get_custom_server_data_folder();
			return std::format("{}\\{}", folder, file_name);
		}

		thread_local size_t buffer_size{};

		int steam_storage_read_file_stub_internal(steam_storage* inst, const char* name, char* buffer, int size)
		{
			console::info("[SteamStorage] Reading file \"%s\"\n", name);

			if (!file_names.contains(name))
			{
				return inst->vftbl->file_read(inst, name, buffer, size);
			}

			const auto path = get_custom_server_data_file_path(name);

			std::string data;
			if (utils::io::read_file(path, &data) && data.size() <= buffer_size)
			{
				std::memcpy(buffer, data.data(), data.size());
				return 1;
			}

			const auto result = inst->vftbl->file_read(inst, name, buffer, size);
			utils::io::write_file(path, std::string{buffer, buffer + size});
			return result;
		}

		void steam_storage_read_file_stub(utils::hook::assembler& a)
		{
			a.mov(rcx, rsi);
			a.mov(qword_ptr(rsp, 0x50), rdi);

			a.mov(rax, rsp);

			a.push(rax);
			a.pushad64();
			a.push(rax);
			a.call_aligned(steam_storage_read_file_stub_internal);
			a.pop(rax);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);

			a.test(eax, eax);
			a.jmp(0x143593E88);
		}

		void* file_read_stub(void* data, const char* file_name, size_t* result_file_size, void* buffer, 
			size_t buf_size, void* a6, void* a7)
		{
			buffer_size = buf_size;
			return file_read_hook.invoke<void*>(data, file_name, result_file_size, buffer, buf_size, a6, a7);
		}

		int file_write_stub(steam_storage* inst, const char* name, const char* buffer, int size)
		{
			console::info("[SteamStorage] Writing file \"%s\"\n", name);

			if (!file_names.contains(name))
			{
				return file_write_hook.invoke<int>(inst, name, buffer, size);
			}

			const auto path = get_custom_server_data_file_path(name);
			if (utils::io::write_file(path, std::string{buffer, buffer + size}))
			{
				return 0;
			}

			return 7;
		}

		std::string get_command_line_args()
		{
			int num_args{};
			const auto argv = CommandLineToArgvW(GetCommandLineW(), &num_args);

			std::string buffer;
			for (auto i = 0; i < num_args; i++)
			{
				buffer.append(utils::string::convert(argv[i]));
				buffer.append(" ");
			}

			LocalFree(argv);
			return buffer;
		}


		HANDLE create_file_stub(LPCWSTR file_name, DWORD desired_access, DWORD share_mode, 
			LPSECURITY_ATTRIBUTES security_attributes, DWORD creation_disp, DWORD flags, HANDLE template_file)
		{
			const auto get_new_path = [&]()
				-> std::wstring
			{
				const auto name = utils::string::convert(file_name);
				const auto base_name = name.substr(name.find_last_of("/\\") + 1);

				if (!file_names.contains(base_name))
				{
					return file_name;
				}

				console::info("[LocalStorage] Create file \"%s\"\n", base_name.data());

				const auto path = get_custom_server_data_file_path(base_name);
				return utils::string::convert(path);
			};

			const auto new_path = get_new_path();
			return CreateFileW(new_path.data(), desired_access, share_mode, security_attributes, creation_disp, flags, template_file);
		}
	}

	bool is_using_custom_server()
	{
		return custom_url[0] != 0;
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			var_custom_server_tpp = vars::register_string("net_custom_server_tpp", "", vars::var_flag_saved | vars::var_flag_latched, "custom server url (tpp)");
			var_custom_server_mgo = vars::register_string("net_custom_server_mgo", "", vars::var_flag_saved | vars::var_flag_latched, "custom server url (mgo)");
		}

		void start() override
		{
			const auto& custom_server_var = SELECT_VALUE(var_custom_server_tpp, var_custom_server_mgo);
			const auto custom_server = custom_server_var->current.get_string();

			if (custom_server.empty())
			{
				return;
			}

			std::memcpy(custom_url, custom_server.data(), custom_server.size());
			console::info("[net] using server url: \"%s\"\n", custom_url);

			if (game::environment::is_tpp())
			{
				file_read_hook.create(0x143593E20, file_read_stub);
				file_write_hook.create(0x143596770, file_write_stub);

				utils::hook::set(0x14DB4F0B8, create_file_stub);

				const auto folder = get_custom_server_data_folder();
				utils::io::write_file(std::format("{}\\server_url.txt", folder), custom_url);

				utils::hook::jump(0x143593E7B, utils::hook::assemble(steam_storage_read_file_stub), true);
			}

			utils::hook::inject(SELECT_VALUE(0x1407D27AC, 0x140572AD6) + 3, custom_url);
		}
	};
}

REGISTER_COMPONENT(custom_server::component)
