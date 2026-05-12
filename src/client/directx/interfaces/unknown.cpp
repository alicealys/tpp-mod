#include <std_include.hpp>

#include "unknown.hpp"

#include "dxgi/dxgi_device.hpp"
#include "dxgi/dxgi_adapter.hpp"
#include "d3d11/d3d11_user_defined_annotation.hpp"

namespace directx
{
	namespace
	{
		HRESULT __stdcall query_interface(void*, IID* iid, void** res)
		{
			switch (iid->Data1)
			{
			case 0x77DB970F: // IID_IDXGIDevice1
				*res = dxgi_device::get_instance();
				return S_OK;
			case 0x29038F61: // IID_IDXGIAdapter1
				*res = dxgi_adapter::get_instance();
				return S_OK;
			case 0xB2DAAD8B:
				*res = d3d11_user_defined_annotation::get_instance();
				return S_OK;
			}

			*res = NULL;
			return DXGI_ERROR_NOT_FOUND;
		}

		ULONG __stdcall add_ref(void*)
		{
			return 0;
		}

		ULONG __stdcall release(void*)
		{
			return 0;
		}
	}

	void unknown::initialize(unknown::vtable* instance)
	{
		instance->query_interface = query_interface;
		instance->add_ref = add_ref;
		instance->release = release;
	}
}
