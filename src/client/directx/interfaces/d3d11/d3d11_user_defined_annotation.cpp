#include <std_include.hpp>

#include "d3d11_user_defined_annotation.hpp"

namespace directx
{
	namespace
	{
		INT __stdcall begin_event(void*, LPCWSTR Name)
		{
			return 0;
		}

		void __stdcall set_marker(void*, LPCWSTR Name)
		{

		}

		BOOL __stdcall get_status(void*)
		{
			return TRUE;
		}
	}

	d3d11_user_defined_annotation* d3d11_user_defined_annotation::create()
	{
		static d3d11_user_defined_annotation instance{};
		static d3d11_user_defined_annotation::vtable vtable{};
		instance.__vftable = &vtable;

		unknown::initialize(&vtable);

		vtable.begin_event = begin_event;
		vtable.set_marker = set_marker;
		vtable.get_status = get_status;
		
		return &instance;
	}
		
	d3d11_user_defined_annotation* d3d11_user_defined_annotation::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
