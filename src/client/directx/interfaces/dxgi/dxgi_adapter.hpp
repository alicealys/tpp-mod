#pragma once

#include "dxgi_object.hpp"

namespace directx
{
	struct dxgi_adapter
	{
		struct vtable : dxgi_object::vtable
		{
			HRESULT (__stdcall* enum_outputs)(void*, UINT, IDXGIOutput**);
			HRESULT (__stdcall* get_desc)(void*, DXGI_ADAPTER_DESC*);
			HRESULT (__stdcall* check_interface_support)(void*, GUID*, LARGE_INTEGER*);
			HRESULT (__stdcall* get_desc1)(void*, DXGI_ADAPTER_DESC*);
		};

		vtable* __vftable;

		static dxgi_adapter* create();
		static dxgi_adapter* get_instance();

		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	
	};

}
