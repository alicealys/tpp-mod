#include <std_include.hpp>

#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include <utils/nt.hpp>

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

	int start()
	{
		enable_dpi_awareness();

		component_loader::pre_load();
		component_loader::post_load();
		component_loader::start();
		return 1;
	}

	void destroy()
	{
		component_loader::end();
	}
}

BOOL WINAPI DllMain(HINSTANCE, const DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		if (!start())
		{
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

#define EXPORT_FUNC(__name__, __return__) \
	extern "C" __declspec(dllexport)  __return__ __stdcall __name__() { get_dinput_library().invoke<__return__>(#__name__); } \

//EXPORT_FUNC(DllCanUnloadNow, void);
//EXPORT_FUNC(DllGetClassObject, void);
//EXPORT_FUNC(DllRegisterServer, void);
//EXPORT_FUNC(DllUnregisterServer, void);
EXPORT_FUNC(GetdfDIJoystick, void);
