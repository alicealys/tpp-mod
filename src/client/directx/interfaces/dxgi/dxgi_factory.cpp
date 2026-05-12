#include <std_include.hpp>

#include "dxgi_factory.hpp"
#include "dxgi_adapter.hpp"
#include "dxgi_swap_chain.hpp"

namespace directx
{
    namespace
    {
        HRESULT __stdcall enum_adapters(void*, UINT index, IDXGIAdapter** adapter)
        {
            if (index > 1)
            {
                *adapter = NULL;
                return DXGI_ERROR_NOT_FOUND;
            }

            *adapter = dxgi_adapter::get<IDXGIAdapter>();
            return S_OK;
        }

        HRESULT __stdcall make_window_association(void*, HWND, UINT)
        {
            return S_OK;
        }

        HRESULT __stdcall get_window_association(void*, HWND* hwnd)
        {
            return S_OK;
        }

        HRESULT __stdcall create_swap_chain(void*, IUnknown*, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain** swap_chain)
        {
            *swap_chain = dxgi_swap_chain::get<IDXGISwapChain>();
            return S_OK;
        }

        HRESULT __stdcall create_software_adapter(void*, HMODULE, IDXGIAdapter**)
        {
            return DXGI_ERROR_INVALID_CALL;
        }

        BOOL __stdcall is_current(void*)
        {
            return FALSE;
        }
    }

    dxgi_factory* dxgi_factory::create()
    {
        static dxgi_factory instance{};
        static dxgi_factory::vtable vtable{};
        instance.__vftable = &vtable;

        dxgi_object::initialize(&vtable);
        vtable.enum_adapters = enum_adapters;
        vtable.make_window_association = make_window_association;
        vtable.get_window_association = get_window_association;
        vtable.create_swap_chain = create_swap_chain;
        vtable.create_software_adapter = create_software_adapter;
        vtable.enum_adapters1 = enum_adapters;
        vtable.is_current = is_current;

        return &instance;
    }

    dxgi_factory* dxgi_factory::get_instance()
    {
        static const auto instance = create();
        return instance;
    }
}
