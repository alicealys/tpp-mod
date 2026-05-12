#include <std_include.hpp>

#include "d3d11_buffer.hpp"

namespace directx
{
	namespace
	{
		void __stdcall get_desc(void*, D3D11_BUFFER_DESC* desc)
		{

		}
	}

	d3d11_buffer* d3d11_buffer::create()
	{
		static d3d11_buffer instance{};
		static d3d11_buffer::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_device_child::initialize(&vtable);

		vtable.get_desc = get_desc;
		
		return &instance;
	}
		
	d3d11_buffer* d3d11_buffer::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
