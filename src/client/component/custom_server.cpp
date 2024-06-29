#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "scheduler.hpp"
#include "console.hpp"

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
			{"TPP_GAME_DATA"}
		};

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
	}

	bool is_using_custom_server()
	{
		return custom_url[0] != 0;
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			const auto custom_server = utils::flags::get_flag("custom-server");
			if (!custom_server.has_value() || custom_server->size() > sizeof(custom_url))
			{
				return;
			}

			file_read_hook.create(0x143593E20, file_read_stub);
			file_write_hook.create(0x143596770, file_write_stub);

			std::memcpy(custom_url, custom_server->data(), custom_server->size());
			console::info("Using custom server url: %s\n", custom_url);

			const auto folder = get_custom_server_data_folder();
			utils::io::write_file(std::format("{}\\server_url.txt", folder), custom_url);

			utils::hook::inject(0x1407D27AC + 3, custom_url);
			utils::hook::jump(0x143593E7B, utils::hook::assemble(steam_storage_read_file_stub), true);
		}
	};
}

REGISTER_COMPONENT(custom_server::component)
