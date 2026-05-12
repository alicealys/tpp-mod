#include <std_include.hpp>

#include "../d3d11/d3d11_texture2d.hpp"
#include "dxgi_output.hpp"
#include "dxgi_device.hpp"
#include "dxgi_swap_chain.hpp"

namespace directx
{
    namespace
    {
        HRESULT __stdcall present(void*, UINT, UINT)
        {
            return S_OK;
        }

        HRESULT __stdcall get_buffer(void*, UINT, IID* iid, void** buffer)
        {
            switch (iid->Data1)
            {
            case 0x6F15AAF2: // IID_ID3D11Texture2D
                *buffer = d3d11_texture2d::get_instance();
                return S_OK;
            }

            *buffer = NULL;
            return E_NOTIMPL;
        }

        HRESULT __stdcall set_fullscreen_state(void*, BOOL, IDXGIOutput*)
        {
            return S_OK;
        }

        HRESULT __stdcall get_fullscreen_state(void*, BOOL*, IDXGIOutput** output)
        {
            *output = dxgi_output::get<IDXGIOutput>();
            return S_OK;
        }

        HRESULT __stdcall get_desc(void*, DXGI_SWAP_CHAIN_DESC* desc)
        {
            return S_OK;
        }

        HRESULT __stdcall resize_buffers(void*, UINT, UINT, UINT, DXGI_FORMAT, UINT)
        {
            return S_OK;
        }

        HRESULT __stdcall resize_targets(void*, DXGI_MODE_DESC*)
        {
            return S_OK;
        }

        HRESULT __stdcall get_containing_output(void*, IDXGIOutput** output)
        {
            *output = dxgi_output::get<IDXGIOutput>();
            return S_OK;
        }

        HRESULT __stdcall get_frame_statistics(void*, DXGI_FRAME_STATISTICS*)
        {
            return S_OK;
        }

        HRESULT __stdcall get_last_present_count(void*, UINT* count)
        {
            *count = 0;
            return S_OK;
        }
    }

    dxgi_swap_chain* dxgi_swap_chain::create()
    {
        static dxgi_swap_chain instance{};
        static dxgi_swap_chain::vtable vtable{};
        instance.__vftable = &vtable;

        dxgi_device_sub_object::initialize(&vtable);
        vtable.present = present;
        vtable.get_buffer = get_buffer;
        vtable.set_fullscreen_state = set_fullscreen_state;
        vtable.get_fullscreen_state = get_fullscreen_state;
        vtable.get_desc = get_desc;
        vtable.resize_buffers = resize_buffers;
        vtable.resize_targets = resize_targets;
        vtable.get_containing_output = get_containing_output;
        vtable.get_frame_statistics = get_frame_statistics;
        vtable.get_last_present_count = get_last_present_count;

        return &instance;
    }

    dxgi_swap_chain* dxgi_swap_chain::get_instance()
    {
        static const auto instance = create();
        return instance;
    }
}
