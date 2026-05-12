#include <std_include.hpp>

#include "d3d11_counter.hpp"

namespace directx
{
	namespace
	{
		UINT __stdcall get_data_size(void*)
		{
			return 0;
		}

		void __stdcall get_desc(void*, D3D11_COUNTER_DESC*)
		{

		}
	}

	d3d11_counter* d3d11_counter::create()
	{
		static d3d11_counter instance{};
		static d3d11_counter::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_device_child::initialize(&vtable);

		vtable.get_data_size = get_data_size;
		vtable.get_desc = get_desc;
		
		return &instance;
	}
		
	d3d11_counter* d3d11_counter::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
