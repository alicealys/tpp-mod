#pragma once

#include "dxgi_object.hpp"

namespace directx
{
	struct dxgi_device
	{
        struct vtable : dxgi_object::vtable
        {
            HRESULT (__stdcall* get_adapter)(void*, IDXGIAdapter**);
            HRESULT (__stdcall* create_surface)(void*, DXGI_SURFACE_DESC*, UINT, DXGI_USAGE, DXGI_SHARED_RESOURCE*, IDXGISurface**);
            HRESULT (__stdcall* query_resource_residency)(void*, IUnknown**, DXGI_RESIDENCY*, UINT);
            HRESULT (__stdcall* set_gpu_thread_priority)(void*, INT);
            HRESULT (__stdcall* get_gpu_thread_priority)(void*, INT*);
        };

        vtable* __vftable;

        static dxgi_device* create();
        static dxgi_device* get_instance();

        template <typename T>
        static T* get()
        {
            return reinterpret_cast<T*>(get_instance());
        }
	
	};
}
