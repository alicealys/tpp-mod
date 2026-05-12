#include <std_include.hpp>

#include "d3d11_rasterizer_state.hpp"

namespace directx
{
	namespace
	{
		void __stdcall get_desc(void*, D3D11_RASTERIZER_DESC* desc)
		{

		}
	}

	d3d11_rasterizer_state* d3d11_rasterizer_state::create()
	{
		static d3d11_rasterizer_state instance{};
		static d3d11_rasterizer_state::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_device_child::initialize(&vtable);
		
		vtable.get_desc = get_desc;

		return &instance;
	}
		
	d3d11_rasterizer_state* d3d11_rasterizer_state::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
