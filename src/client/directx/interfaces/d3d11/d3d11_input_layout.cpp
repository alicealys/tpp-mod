#include <std_include.hpp>

#include "d3d11_input_layout.hpp"

namespace directx
{
	namespace
	{
	
	}

	d3d11_input_layout* d3d11_input_layout::create()
	{
		static d3d11_input_layout instance{};
		static d3d11_input_layout::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_device_child::initialize(&vtable);
		
		return &instance;
	}
		
	d3d11_input_layout* d3d11_input_layout::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
