#pragma once

#include "d3d11_device_child.hpp"
#include "d3d11_resource.hpp"

namespace directx
{
	struct d3d11_texture2d
	{
		struct vtable : d3d11_resource::vtable
		{
			void (__stdcall* get_desc)(void*, D3D11_TEXTURE2D_DESC*);
		};

		vtable* __vftable;

		static d3d11_texture2d* create();
		static d3d11_texture2d* get_instance();
		
		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	};
}
