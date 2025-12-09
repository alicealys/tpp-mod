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
	
	bool check_version()
	{
		utils::nt::library self;
		const auto path = self.get_path();
		std::string data;
		OutputDebugString(path.data());
		if (!utils::io::read_file(path, &data))
		{
			return false;
		}

		const auto hash = utils::cryptography::sha1::compute(data, true);
		if (hash != "17AC94A4BC9F88B035A45122C8A67EFA38D03F2A" /*tpp*/ &&
			hash != "8690C7C27C94DD6A452BA6A612B7B485918F3BAF" /*mgo*/)
		{
			return false;
		}

		return true;
	}

	int start()
	{
		if (!check_version())
		{
			MessageBoxA(nullptr, "This version of TPP or MGO is not supported", "TPP-Mod: ERROR", MB_ICONERROR);
			std::exit(0);
		}

		enable_dpi_awareness();

		component_loader::pre_load();
		component_loader::post_load();
		component_loader::start();
		component_loader::post_start();
		return 1;
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
