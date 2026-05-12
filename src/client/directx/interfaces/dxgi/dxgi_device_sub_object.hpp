#pragma once

#include "dxgi_object.hpp"

namespace directx
{
	struct dxgi_device_sub_object
	{
        struct vtable : dxgi_object::vtable
        {
            HRESULT (__stdcall* get_device)(void*, IID*, void**);
        };

        static void initialize(dxgi_device_sub_object::vtable* instance);
	};
}
