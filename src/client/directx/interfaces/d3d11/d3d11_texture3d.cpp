#include <std_include.hpp>

#include "d3d11_texture3d.hpp"

namespace directx
{
	namespace
	{
		void __stdcall get_desc(void*, D3D11_TEXTURE3D_DESC* desc)
		{

		}
	}

	d3d11_texture3d* d3d11_texture3d::create()
	{
		static d3d11_texture3d instance{};
		static d3d11_texture3d::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_resource::initialize(&vtable);
		vtable.get_desc = get_desc;

		return &instance;
	}
		
	d3d11_texture3d* d3d11_texture3d::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
