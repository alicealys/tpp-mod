#include <std_include.hpp>

#include "d3d11_depth_stencil_state.hpp"

namespace directx
{
	namespace
	{
		void __stdcall get_desc(void*, D3D11_DEPTH_STENCIL_DESC*)
		{

		}
	}

	d3d11_depth_stencil_state* d3d11_depth_stencil_state::create()
	{
		static d3d11_depth_stencil_state instance{};
		static d3d11_depth_stencil_state::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_device_child::initialize(&vtable);
		
		vtable.get_desc = get_desc;

		return &instance;
	}
		
	d3d11_depth_stencil_state* d3d11_depth_stencil_state::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
