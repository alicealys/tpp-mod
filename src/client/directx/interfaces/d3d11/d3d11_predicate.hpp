#pragma once

#include "d3d11_device_child.hpp"
#include "d3d11_query.hpp"

namespace directx
{
	struct d3d11_predicate
	{
		struct vtable : public d3d11_query::vtable
		{

		};

		vtable* __vftable;

		static d3d11_predicate* create();
		static d3d11_predicate* get_instance();
		
		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	};
}
