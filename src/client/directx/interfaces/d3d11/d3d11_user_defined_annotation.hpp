#pragma once

#include "d3d11_device_child.hpp"

namespace directx
{
	struct d3d11_user_defined_annotation
	{
		struct vtable : public unknown::vtable
		{
			INT (__stdcall* begin_event)(void*, LPCWSTR Name);
			void (__stdcall* set_marker)(void*, LPCWSTR Name);
			BOOL (__stdcall* get_status)(void*);
		};

		vtable* __vftable;

		static d3d11_user_defined_annotation* create();
		static d3d11_user_defined_annotation* get_instance();
		
		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	};
}
