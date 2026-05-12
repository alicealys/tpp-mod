#include <std_include.hpp>

#include "d3d11_resource.hpp"
#include "d3d11_render_target_view.hpp"

namespace directx
{
	namespace
	{
		void __stdcall get_desc(void*, D3D11_RENDER_TARGET_VIEW_DESC*)
		{

		}
	}

	d3d11_render_target_view* d3d11_render_target_view::create()
	{
		static d3d11_render_target_view instance{};
		static d3d11_render_target_view::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_view::initialize(&vtable);

		vtable.get_desc = get_desc;
		
		return &instance;
	}
		
	d3d11_render_target_view* d3d11_render_target_view::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
