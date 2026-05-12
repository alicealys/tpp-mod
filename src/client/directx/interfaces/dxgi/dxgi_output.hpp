#pragma once

#include "dxgi_object.hpp"

namespace directx
{
	struct dxgi_output
	{
        struct vtable : dxgi_object::vtable
        {
            HRESULT (__stdcall* get_desc)(void*, DXGI_OUTPUT_DESC* pDesc);
            HRESULT (__stdcall* get_display_mode_list)(void*, DXGI_FORMAT, UINT, UINT*, DXGI_MODE_DESC*);
            HRESULT (__stdcall* find_closest_matching_mode)(void*, DXGI_MODE_DESC*, DXGI_MODE_DESC*, IUnknown*);
            HRESULT (__stdcall* wait_for_vblank)(void*);
            HRESULT (__stdcall* take_ownership)(void*, IUnknown* pDevice, BOOL Exclusive);
            void (__stdcall* release_ownership)(void*);
            HRESULT (__stdcall* get_gamma_control_capabilities)(void*, DXGI_GAMMA_CONTROL_CAPABILITIES*);
            HRESULT (__stdcall* set_gamma_control)(void*, DXGI_GAMMA_CONTROL*);
            HRESULT (__stdcall* get_gamma_control)(void*, DXGI_GAMMA_CONTROL*);
            HRESULT (__stdcall* set_display_surface)(void*, IDXGISurface*);
            HRESULT (__stdcall* get_display_surface_data)(void*, IDXGISurface*);
            HRESULT (__stdcall* get_frame_statistics)(void*, DXGI_FRAME_STATISTICS*);
       
        };

        vtable* __vftable;

        static dxgi_output* create();
        static dxgi_output* get_instance();

        template <typename T>
        static T* get()
        {
            return reinterpret_cast<T*>(get_instance());
        }
	
	};

    static_assert(offsetof(dxgi_output::vtable, get_desc) == 0x38);
    static_assert(offsetof(dxgi_output::vtable, get_display_mode_list) == 0x40);
}
