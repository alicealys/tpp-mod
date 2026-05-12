#pragma once

#include "d3d11_device_child.hpp"

namespace directx
{
	struct d3d11_sampler_state
	{
		struct vtable : d3d11_device_child::vtable
		{
			void(__stdcall* get_desc)(void*, D3D11_SAMPLER_DESC*);
		};

		vtable* __vftable;

		static d3d11_sampler_state* create();
		static d3d11_sampler_state* get_instance();
		
		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	};
}
