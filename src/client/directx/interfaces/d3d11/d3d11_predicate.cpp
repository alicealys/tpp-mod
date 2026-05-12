#include <std_include.hpp>

#include "d3d11_predicate.hpp"

namespace directx
{
	namespace
	{
	
	}

	d3d11_predicate* d3d11_predicate::create()
	{
		static d3d11_predicate instance{};
		static d3d11_predicate::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_device_child::initialize(&vtable);
		
		return &instance;
	}
		
	d3d11_predicate* d3d11_predicate::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
