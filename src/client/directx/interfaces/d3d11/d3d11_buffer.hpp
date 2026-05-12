#pragma once

#include "d3d11_device_child.hpp"

namespace directx
{
	struct d3d11_buffer
	{
		struct vtable : public d3d11_device_child::vtable
		{
			void (__stdcall* get_desc)(void*, D3D11_BUFFER_DESC*);
		};

		vtable* __vftable;

		static d3d11_buffer* create();
		static d3d11_buffer* get_instance();
		
		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	};
}
