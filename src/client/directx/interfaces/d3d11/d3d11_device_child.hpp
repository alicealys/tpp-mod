#pragma once

#include "../unknown.hpp"

namespace directx
{
	struct d3d11_device_child
	{
		struct vtable : unknown::vtable
		{
            HRESULT (__stdcall* get_device)(void*, ID3D11Device** device);
            HRESULT (__stdcall* get_private_data)(void*, GUID*, UINT*, void*);
            HRESULT (__stdcall* set_private_data)(void*, GUID*, UINT, void*);
            HRESULT (__stdcall* set_private_data_interface)(void*, GUID*, IUnknown*);
		};
	
		static void initialize(d3d11_device_child::vtable*);

	};
}
