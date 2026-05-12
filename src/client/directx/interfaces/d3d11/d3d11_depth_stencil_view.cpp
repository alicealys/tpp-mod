#include <std_include.hpp>

#include "d3d11_depth_stencil_view.hpp"

namespace directx
{
	namespace
	{
		void __stdcall get_desc(void*, D3D11_DEPTH_STENCIL_VIEW_DESC* desc)
		{

		}
	}

	d3d11_depth_stencil_view* d3d11_depth_stencil_view::create()
	{
		static d3d11_depth_stencil_view instance{};
		static d3d11_depth_stencil_view::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_device_child::initialize(&vtable);

		vtable.get_desc = get_desc;
		
		return &instance;
	}
		
	d3d11_depth_stencil_view* d3d11_depth_stencil_view::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
