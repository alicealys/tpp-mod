#include <std_include.hpp>

#include "d3d11_device_child.hpp"
#include "d3d11_device.hpp"

namespace directx
{
	namespace
	{
		HRESULT __stdcall get_device(void*, ID3D11Device** device)
		{
			*device = d3d11_device::get<ID3D11Device>();
			return S_OK;
		}

		HRESULT __stdcall get_private_data(void*, GUID*, UINT* size, void*)
		{
			*size = 0;
			return S_OK;
		}

		HRESULT __stdcall set_private_data(void*, GUID*, UINT, void*)
		{
			return S_OK;
		}

		HRESULT __stdcall set_private_data_interface(void*, GUID*, IUnknown*)
		{
			return S_OK;
		}
	}

	void d3d11_device_child::initialize(d3d11_device_child::vtable* instance)
	{
		unknown::initialize(instance);
		instance->get_device = get_device;
		instance->set_private_data = set_private_data;
		instance->set_private_data_interface = set_private_data_interface;
	}
}
