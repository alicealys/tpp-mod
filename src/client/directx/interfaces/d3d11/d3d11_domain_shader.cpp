#include <std_include.hpp>

#include "d3d11_domain_shader.hpp"

namespace directx
{
	namespace
	{
	
	}

	d3d11_domain_shader* d3d11_domain_shader::create()
	{
		static d3d11_domain_shader instance{};
		static d3d11_domain_shader::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_device_child::initialize(&vtable);
		
		return &instance;
	}
		
	d3d11_domain_shader* d3d11_domain_shader::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
