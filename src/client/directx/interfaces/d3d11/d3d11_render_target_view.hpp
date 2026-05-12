#pragma once

#include "d3d11_view.hpp"

namespace directx
{
	struct d3d11_render_target_view
	{
		struct vtable : public d3d11_view::vtable
		{
			void(__stdcall* get_desc)(void*, D3D11_RENDER_TARGET_VIEW_DESC*);
		};

		vtable* __vftable;

		static d3d11_render_target_view* create();
		static d3d11_render_target_view* get_instance();
		
		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	};
}
