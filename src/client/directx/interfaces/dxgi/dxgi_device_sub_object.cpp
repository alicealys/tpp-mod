#include <std_include.hpp>

#include "dxgi_output.hpp"
#include "dxgi_device.hpp"
#include "dxgi_device_sub_object.hpp"

namespace directx
{
    namespace
    {
        HRESULT __stdcall get_device(void*, IID* iid, void** device)
        {
            switch (iid->Data1)
            {
            case 0x54EC77FA:
                *device = dxgi_device::get_instance();
                return S_OK;
            }

            *device = NULL;
            return DXGI_ERROR_NOT_FOUND;
        }
    }

    void dxgi_device_sub_object::initialize(dxgi_device_sub_object::vtable* instance)
    {
        dxgi_object::initialize(instance);
        instance->get_device = get_device;
    }
}
