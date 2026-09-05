#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>
#include <utils/thread.hpp>
#include <utils/compression.hpp>

#include <exception/minidump.hpp>

#include <version.hpp>

namespace exception
{
	namespace
	{
		thread_local struct
		{
			DWORD code = 0;
			PVOID address = nullptr;
			ULONG_PTR information[15]{};
			char crash_name[MAX_PATH]{};
		} exception_data;

		std::atomic_bool handler_disabled;

		void show_mouse_cursor()
		{
			while (ShowCursor(TRUE) < 0);
		}

		bool get_module(const std::size_t address, HMODULE* module, char* buffer, std::size_t size)
		{
			const auto has_module = GetModuleHandleEx(
				GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
				GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
				reinterpret_cast<LPCSTR>(address),
				module);

			if (has_module)
			{
				GetModuleFileNameA(*module, buffer, static_cast<DWORD>(size));
			}

			return has_module;
		}

		void display_error_dialog()
		{
			HMODULE module{};
			char module_name[MAX_PATH]{};
			const auto has_module = get_module(reinterpret_cast<std::size_t>(exception_data.address),
				&module, module_name, sizeof(module_name));

			std::string error_str = utils::string::va("Exception (0x%08X) at 0x%p in module\n", exception_data.code, exception_data.address);

			if (has_module)
			{
				error_str += utils::string::va("\n%s\n", module_name);
			}
			else
			{
				error_str += "\nunknown module\n";
			}

			if (exception_data.code == EXCEPTION_ACCESS_VIOLATION)
			{
				switch (exception_data.information[0])
				{
				case 0:
					error_str += utils::string::va("\nTried to read from invalid address 0x%p\n", exception_data.information[1]);
					break;
				case 1:
					error_str += utils::string::va("\nTried to write at invalid address 0x%p\n", exception_data.information[1]);
					break;
				default:
					error_str += utils::string::va("\nInvalid address 0x%p\n", exception_data.information[1]);
				}
			}

			error_str += utils::string::va("\nA crash dump has been written at %s\n", exception_data.crash_name),

				utils::thread::suspend_other_threads();
			show_mouse_cursor();

			MessageBoxA(nullptr, error_str.data(), "MGV-Mod ERROR", MB_ICONERROR);
			TerminateProcess(GetCurrentProcess(), exception_data.code);
		}

		void reset_state()
		{
			display_error_dialog();
		}

		size_t get_reset_state_stub()
		{
			static auto* stub = utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.sub(rsp, 0x10);
				a.or_(rsp, 0x8);
				a.jmp(reset_state);
			});

			return reinterpret_cast<size_t>(stub);
		}

		std::string get_timestamp()
		{
			tm ltime{};
			char timestamp[MAX_PATH] = {0};
			const auto time = _time64(nullptr);

			_localtime64_s(&ltime, &time);
			strftime(timestamp, sizeof(timestamp) - 1, "%Y-%m-%d-%H-%M-%S", &ltime);

			return timestamp;
		}

		std::string generate_crash_info(const LPEXCEPTION_POINTERS exception_info)
		{
			std::string info{};
			const auto line = [&info](const std::string& text)
			{
				info.append(text);
				info.append("\r\n");
			};

			HMODULE module{};
			char module_name[MAX_PATH]{};
			get_module(reinterpret_cast<std::size_t>(exception_info->ExceptionRecord->ExceptionAddress),
				&module, module_name, sizeof(module_name));

			line("MGV-MOD Crash Dump");
			line("");
			line("Version: "s + VERSION);
			line("Timestamp: "s + get_timestamp());
			line(utils::string::va("Exception: 0x%08X", exception_info->ExceptionRecord->ExceptionCode));
			line(utils::string::va("Address: 0x%llX", exception_info->ExceptionRecord->ExceptionAddress));
			line(utils::string::va("Module path: %s", module_name));

			return info;
		}

		void write_minidump(const LPEXCEPTION_POINTERS exceptioninfo)
		{
			const auto process_params = NtCurrentTeb()->ProcessEnvironmentBlock->ProcessParameters;
			SecureZeroMemory(process_params->CommandLine.Buffer, process_params->CommandLine.Length);
			process_params->CommandLine.Length = 0;

			const auto timestamp = get_timestamp();
			const auto crash_name = std::format("minidumps/mgv-mod-crash-{}.zip", timestamp);
			strncpy_s(exception_data.crash_name, sizeof(exception_data.crash_name), crash_name.data(), _TRUNCATE);

			utils::compression::zip::archive zip_file{};
			zip_file.add("crash.dmp", create_minidump(exceptioninfo));
			zip_file.add("info.txt", generate_crash_info(exceptioninfo));
			zip_file.write(crash_name, "MGV-Mod Crash Dump");
		}

		bool is_harmless_error(const LPEXCEPTION_POINTERS exception_info)
		{
			const auto code = exception_info->ExceptionRecord->ExceptionCode;
			return code == STATUS_INTEGER_OVERFLOW || code == STATUS_FLOAT_OVERFLOW || code == STATUS_SINGLE_STEP;
		}

		LONG WINAPI exception_filter(const LPEXCEPTION_POINTERS exception_info)
		{
			if (is_harmless_error(exception_info))
			{
				return EXCEPTION_CONTINUE_EXECUTION;
			}

			if (!handler_disabled)
			{
				write_minidump(exception_info);

				exception_data.code = exception_info->ExceptionRecord->ExceptionCode;
				exception_data.address = exception_info->ExceptionRecord->ExceptionAddress;
				std::memcpy(&exception_data.information, exception_info->ExceptionRecord->ExceptionInformation,
					sizeof(exception_info->ExceptionRecord->ExceptionInformation));
				exception_info->ContextRecord->Rip = get_reset_state_stub();

				return EXCEPTION_CONTINUE_EXECUTION;
			}
			else
			{
				return EXCEPTION_CONTINUE_SEARCH;
			}
		}

		LPTOP_LEVEL_EXCEPTION_FILTER WINAPI set_unhandled_exception_filter_stub(LPTOP_LEVEL_EXCEPTION_FILTER)
		{
			return &exception_filter;
		}
	}

	void disable_handler()
	{
		handler_disabled = true;
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			SetUnhandledExceptionFilter(exception_filter);
			utils::hook::jump(SetUnhandledExceptionFilter, set_unhandled_exception_filter_stub, true);

#ifdef DEBUG
			command::add("crash", []()
			{
				*reinterpret_cast<int*>(0) = 1;
			});
#endif
		}

		void game_initialized() override
		{
			SetUnhandledExceptionFilter(exception_filter);
		}
	};
}

REGISTER_COMPONENT(exception::component)
