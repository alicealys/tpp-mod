#pragma once

#include "d3d11_device_child.hpp"

namespace directx
{
	struct d3d11_class_instance
	{
		struct vtable : public d3d11_device_child::vtable
		{
            void (__stdcall* get_class_linkage)(void*, ID3D11ClassLinkage**);
            void (__stdcall* get_desc)(void*, D3D11_CLASS_INSTANCE_DESC*);
            void (__stdcall* get_instance_name)(void*, LPSTR, SIZE_T*);
            void (__stdcall* get_type_name)(void*, LPSTR, SIZE_T*);
		};

		vtable* __vftable;

		static d3d11_class_instance* create();
		static d3d11_class_instance* get_instance();
		
		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	};
}
