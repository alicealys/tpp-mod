#pragma once

#include "d3d11_device_child.hpp"
#include "d3d11_view.hpp"

namespace directx
{
	struct d3d11_unordered_access_view
	{
		struct vtable : public d3d11_view::vtable
		{
			void(__stdcall* get_desc)(void*, D3D11_BUFFER_DESC*);
		};

		vtable* __vftable;

		static d3d11_unordered_access_view* create();
		static d3d11_unordered_access_view* get_instance();
		
		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	};
}
