#include <std_include.hpp>

#include "d3d11_resource.hpp"
#include "d3d11_view.hpp"

namespace directx
{
	namespace
	{
		void __stdcall get_resource(void*, ID3D11Resource** resource)
		{
			*resource = d3d11_resource::get<ID3D11Resource>();
		}
	}

	void d3d11_view::initialize(d3d11_view::vtable* vtable)
	{
		d3d11_device_child::initialize(vtable);
		vtable->get_resource = get_resource;
	}
}
