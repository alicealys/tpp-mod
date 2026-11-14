#include <std_include.hpp>

#include <utils/hook.hpp>

extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 1;
	__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;

	int s_read_arc4random(void*, size_t)
	{
		return -1;
	}

	int s_read_getrandom(void*, size_t)
	{
		return -1;
	}

	int s_read_urandom(void*, size_t)
	{
		return -1;
	}

	int s_read_ltm_rng(void*, size_t)
	{
		return -1;
	}

	__declspec(dllexport) HRESULT WINAPI DirectInput8Create(HINSTANCE inst, DWORD version, REFIID id, LPVOID* out, void* outer)
	{
		static auto lib = utils::nt::load_system_library("dinput8.dll");
		return lib.invoke<HRESULT>("DirectInput8Create", inst, version, id, out, outer);
	}

	__declspec(dllexport) HRESULT WINAPI D3D11CreateDevice(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6, void* a7, 
		void* a8, void* a9, void* a10)
	{
		static auto lib = utils::nt::load_system_library("d3d11.dll");
		return lib.invoke<HRESULT>("D3D11CreateDevice", a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
	}
};
