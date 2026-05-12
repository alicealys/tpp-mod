#include <std_include.hpp>

#include "dxgi_adapter.hpp"
#include "dxgi_device.hpp"

namespace directx
{
    namespace
    {
        HRESULT __stdcall get_adapter(void*, IDXGIAdapter** adapter)
        {
            *adapter = dxgi_adapter::get<IDXGIAdapter>();
            return S_OK;
        }

        HRESULT __stdcall create_surface(void*, DXGI_SURFACE_DESC*, UINT, DXGI_USAGE, DXGI_SHARED_RESOURCE*, IDXGISurface**)
        {
            return E_NOTIMPL;
        }

        HRESULT __stdcall query_resource_residency(void*, IUnknown**, DXGI_RESIDENCY*, UINT)
        {
            return E_NOTIMPL;
        }

        HRESULT __stdcall set_gpu_thread_priority(void*, INT)
        {
            return S_OK;
        }

        HRESULT __stdcall get_gpu_thread_priority(void*, INT* priority)
        {
            *priority = 0;
            return S_OK;
        }
    }

    dxgi_device* dxgi_device::create()
    {
        static dxgi_device instance{};
        static dxgi_device::vtable vtable{};
        instance.__vftable = &vtable;

        dxgi_object::initialize(&vtable);

        vtable.get_adapter = get_adapter;
        vtable.create_surface = create_surface;
        vtable.query_resource_residency = query_resource_residency;
        vtable.set_gpu_thread_priority = set_gpu_thread_priority;
        vtable.get_gpu_thread_priority = get_gpu_thread_priority;

        return &instance;
    }

    dxgi_device* dxgi_device::get_instance()
    {
        static const auto instance = create();
        return instance;
    }
}
