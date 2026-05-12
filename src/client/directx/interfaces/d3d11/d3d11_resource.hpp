#pragma once

#include "d3d11_device_child.hpp"

namespace directx
{
	struct d3d11_resource
	{
		struct vtable : d3d11_device_child::vtable
		{
			void(__stdcall* get_type)(void*, D3D11_RESOURCE_DIMENSION*);
			void(__stdcall* set_eviction_priority)(void*, UINT);
			UINT(__stdcall* get_eviction_priority)(void*);
		};

		vtable* __vftable;

		static void initialize(d3d11_resource::vtable*);
		static d3d11_resource* create();
		static d3d11_resource* get_instance();
		
		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	};
}
