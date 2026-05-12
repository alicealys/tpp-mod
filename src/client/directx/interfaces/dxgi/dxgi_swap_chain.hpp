#pragma once

#include "dxgi_object.hpp"
#include "dxgi_device_sub_object.hpp"

namespace directx
{
	struct dxgi_swap_chain
	{
        struct vtable : dxgi_device_sub_object::vtable
        {
            HRESULT (__stdcall* present)(void*, UINT, UINT);
            HRESULT (__stdcall* get_buffer)(void*, UINT, IID*, void**);
            HRESULT (__stdcall* set_fullscreen_state)(void*, BOOL, IDXGIOutput*);
            HRESULT (__stdcall* get_fullscreen_state)(void*, BOOL*, IDXGIOutput**);
            HRESULT (__stdcall* get_desc)(void*, DXGI_SWAP_CHAIN_DESC*);
            HRESULT (__stdcall* resize_buffers)(void*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
            HRESULT (__stdcall* resize_targets)(void*, DXGI_MODE_DESC*);
            HRESULT (__stdcall* get_containing_output)(void*, IDXGIOutput**);
            HRESULT (__stdcall* get_frame_statistics)(void*, DXGI_FRAME_STATISTICS*);
            HRESULT (__stdcall* get_last_present_count)(void*, UINT*);
        };

        vtable* __vftable;

        static dxgi_swap_chain* create();
        static dxgi_swap_chain* get_instance();

        template <typename T>
        static T* get()
        {
            return reinterpret_cast<T*>(get_instance());
        }
	
	};
}
