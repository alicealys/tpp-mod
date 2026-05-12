#include <std_include.hpp>

#include "d3d11_texture2d.hpp"

namespace directx
{
	namespace
	{
		void __stdcall get_desc(void*, D3D11_TEXTURE2D_DESC* desc)
		{

		}
	}

	d3d11_texture2d* d3d11_texture2d::create()
	{
		static d3d11_texture2d instance{};
		static d3d11_texture2d::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_resource::initialize(&vtable);
		vtable.get_desc = get_desc;

		return &instance;
	}
		
	d3d11_texture2d* d3d11_texture2d::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
