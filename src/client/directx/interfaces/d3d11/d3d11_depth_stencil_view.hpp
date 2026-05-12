#pragma once

#include "d3d11_device_child.hpp"

namespace directx
{
	struct d3d11_depth_stencil_view
	{
		struct vtable : public d3d11_device_child::vtable
		{
			void(__stdcall* get_desc)(void*, D3D11_DEPTH_STENCIL_VIEW_DESC*);
		};

		vtable* __vftable;

		static d3d11_depth_stencil_view* create();
		static d3d11_depth_stencil_view* get_instance();
		
		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	};
}
