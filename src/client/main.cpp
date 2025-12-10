#include <std_include.hpp>

#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include <utils/nt.hpp>
#include <utils/io.hpp>
#include <utils/cryptography.hpp>

namespace
{
	utils::nt::library load_dinput_library()
	{
		char dir[MAX_PATH]{};
		GetSystemDirectoryA(dir, sizeof(dir));
		return utils::nt::library::load(dir + "/dinput8.dll"s);
	}

	utils::nt::library& get_dinput_library()
	{
		static auto lib = load_dinput_library();
		return lib;
	}

	void enable_dpi_awareness()
	{
		const utils::nt::library user32{"user32.dll"};
		user32.invoke<BOOL>("SetProcessDpiAwarenessContext", DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	}

	void start()
	{
		enable_dpi_awareness();
		game::environment::detect_version();

		if (!component_loader::pre_load() || !component_loader::post_load())
		{
			throw std::runtime_error("premature shutdown");
		}

		component_loader::start();
		component_loader::post_start();
	}

	void destroy()
	{
		component_loader::end();
	}
}

BOOL WINAPI DllMain(HINSTANCE handle, const DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		utils::nt::library::set_current_handle(handle);

		try
		{
			start();
		}
		catch (const std::exception& e)
		{
			MessageBoxA(nullptr, utils::string::va(
				"Failed to initialize TPP-Mod!\n%s\n\nMake sure you are running the latest steam release and latest TPP-Mod version", 
				e.what()), "TPP-Mod: ERROR", MB_ICONERROR);
			return FALSE;
		}
	}

	if (reason == DLL_PROCESS_DETACH)
	{
		destroy();
	}

	return TRUE;
}

extern "C" __declspec(dllexport) HRESULT DirectInput8Create(HINSTANCE inst, DWORD version, REFIID id, LPVOID* out, void* outer)
{
	return get_dinput_library().invoke<HRESULT>("DirectInput8Create", inst, version, id, out, outer);
}
