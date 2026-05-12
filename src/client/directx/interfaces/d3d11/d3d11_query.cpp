#include <std_include.hpp>

#include "d3d11_query.hpp"

namespace directx
{
	namespace
	{
		void __stdcall get_desc(void*, D3D11_QUERY_DESC* desc)
		{

		}
	}

	d3d11_query* d3d11_query::create()
	{
		static d3d11_query instance{};
		static d3d11_query::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_device_child::initialize(&vtable);

		vtable.get_desc = get_desc;
		
		return &instance;
	}
		
	d3d11_query* d3d11_query::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
