#pragma once

#include "d3d11_device_child.hpp"

namespace directx
{
	struct d3d11_view
	{
		struct vtable : public d3d11_device_child::vtable
		{
			void(__stdcall* get_resource)(void*, ID3D11Resource**);
		};

		static void initialize(d3d11_view::vtable*);
	};
}
