#pragma once

#include "../unknown.hpp"
#include "dxgi_object.hpp"

namespace directx
{
	struct dxgi_object
	{
		struct vtable : unknown::vtable
		{
			HRESULT (__stdcall* set_private_data)(void*, GUID*, UINT, void*);
			HRESULT (__stdcall* set_private_data_interface)(void*, GUID*, IUnknown*);
			HRESULT (__stdcall* get_private_data)(void*, GUID* name, UINT*, void*);
			HRESULT (__stdcall* get_parent)(void*, IID*, void**);
		};
	
		static void initialize(dxgi_object::vtable* instance);
	};
}
