#pragma once

#include "d3d11_device_child.hpp"

namespace directx
{
	struct d3d11_class_linkage
	{
		struct vtable : public d3d11_device_child::vtable
		{
			HRESULT (__stdcall* get_class_instance)(void*, LPCSTR, UINT, ID3D11ClassInstance**);
            HRESULT (__stdcall* create_class_instance)(void*, LPCSTR, UINT, UINT, UINT, UINT, ID3D11ClassInstance**);
		};

		vtable* __vftable;

		static d3d11_class_linkage* create();
		static d3d11_class_linkage* get_instance();
		
		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	};
}
