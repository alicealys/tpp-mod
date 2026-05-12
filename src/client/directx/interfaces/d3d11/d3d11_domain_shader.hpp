#pragma once

#include "d3d11_device_child.hpp"

namespace directx
{
	struct d3d11_domain_shader
	{
		struct vtable : public d3d11_device_child::vtable
		{

		};

		vtable* __vftable;

		static d3d11_domain_shader* create();
		static d3d11_domain_shader* get_instance();
		
		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	};
}
