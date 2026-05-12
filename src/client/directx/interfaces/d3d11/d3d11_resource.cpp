#include <std_include.hpp>

#include "d3d11_resource.hpp"

namespace directx
{
	namespace
	{
		void __stdcall get_type(void*, D3D11_RESOURCE_DIMENSION* dim)
		{
			*dim = D3D11_RESOURCE_DIMENSION_UNKNOWN;
		}

		void __stdcall set_eviction_priority(void*, UINT)
		{

		}

		UINT __stdcall get_eviction_priority(void*)
		{
			return DXGI_RESOURCE_PRIORITY_MINIMUM;
		}
	}

	void d3d11_resource::initialize(d3d11_resource::vtable* vtable)
	{
		d3d11_device_child::initialize(vtable);
		vtable->get_type = get_type;
		vtable->set_eviction_priority = set_eviction_priority;
		vtable->get_eviction_priority = get_eviction_priority;
	}

	d3d11_resource* d3d11_resource::create()
	{
		static d3d11_resource instance{};
		static d3d11_resource::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_resource::initialize(&vtable);
		
		return &instance;
	}
		
	d3d11_resource* d3d11_resource::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
