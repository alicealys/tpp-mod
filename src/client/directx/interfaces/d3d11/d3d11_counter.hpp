#pragma once

#include "d3d11_device_child.hpp"

namespace directx
{
	struct d3d11_counter
	{
		struct vtable : public d3d11_device_child::vtable
		{
			UINT (__stdcall* get_data_size)(void*);
			void( __stdcall* get_desc)(void*, D3D11_COUNTER_DESC*);
		};

		vtable* __vftable;

		static d3d11_counter* create();
		static d3d11_counter* get_instance();
		
		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	};
}
