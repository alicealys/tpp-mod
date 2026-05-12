#include <std_include.hpp>

#include "dxgi_object.hpp"
#include "dxgi_factory.hpp"

namespace directx
{
	namespace
	{
		HRESULT __stdcall set_private_data(void*, GUID*, UINT, void*)
		{
			return S_OK;
		}

		HRESULT __stdcall set_private_data_interface(void*, GUID*, IUnknown*)
		{
			return S_OK;
		}

		HRESULT __stdcall get_private_data(void*, GUID*, UINT* size, void*)
		{
			*size = 0;
			return S_OK;
		}

		HRESULT __stdcall get_parent(void*, IID* iid, void** res)
		{
			switch (iid->Data1)
			{
			case 0x770AAE78: // IID_IDXGIFactory1
				*res = dxgi_factory::get_instance();
				return S_OK;
			}

			*res = NULL;
			return DXGI_ERROR_NOT_FOUND;
		}
	}

	void dxgi_object::initialize(dxgi_object::vtable* instance)
	{
		unknown::initialize(instance);
		instance->set_private_data = set_private_data;
		instance->set_private_data_interface = set_private_data_interface;
		instance->get_private_data = get_private_data;
		instance->get_parent = get_parent;
	}
}
