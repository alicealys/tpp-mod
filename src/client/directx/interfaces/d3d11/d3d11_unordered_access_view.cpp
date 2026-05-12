#include <std_include.hpp>

#include "d3d11_unordered_access_view.hpp"

namespace directx
{
	namespace
	{
		void __stdcall get_desc(void*, D3D11_BUFFER_DESC* desc)
		{

		}
	}

	d3d11_unordered_access_view* d3d11_unordered_access_view::create()
	{
		static d3d11_unordered_access_view instance{};
		static d3d11_unordered_access_view::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_view::initialize(&vtable);
		
		vtable.get_desc = get_desc;

		return &instance;
	}
		
	d3d11_unordered_access_view* d3d11_unordered_access_view::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
