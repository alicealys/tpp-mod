#include <std_include.hpp>

#include "dxgi_output.hpp"

namespace directx
{
    namespace
    {
        HRESULT __stdcall get_desc(void*, DXGI_OUTPUT_DESC* desc)
        {
            std::memset(desc, 0, sizeof(DXGI_OUTPUT_DESC));
            desc->AttachedToDesktop = TRUE;
            desc->DesktopCoordinates.left = 0;
            desc->DesktopCoordinates.top = 0;
            desc->DesktopCoordinates.right = 1;
            desc->DesktopCoordinates.bottom = 1;
            desc->Monitor = 0;
            desc->Rotation = DXGI_MODE_ROTATION_IDENTITY;
            return S_OK;
        }

        HRESULT __stdcall get_display_mode_list(void*, DXGI_FORMAT, UINT, UINT* num, DXGI_MODE_DESC* desc)
        {
            if (desc == nullptr)
            {
                *num = 1;
            }
            else
            {
                for (auto i = 0u; i < *num; i++)
                {
                    std::memset(&desc[i], 0, sizeof(DXGI_MODE_DESC));
                    desc[i].Width = 1;
                    desc[i].Height = 1;
                    desc[i].RefreshRate.Numerator = 60;
                    desc[i].RefreshRate.Denominator = 1;
                    desc[i].ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
                    desc[i].Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
                    desc[i].Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
                }
            }

            return S_OK;
        }

        HRESULT __stdcall find_closest_matching_mode(void*, DXGI_MODE_DESC*, DXGI_MODE_DESC*, IUnknown*)
        {
            return DXGI_ERROR_NOT_FOUND;
        }

        HRESULT __stdcall wait_for_vblank(void*)
        {
            return S_OK;
        }

        HRESULT __stdcall take_ownership(void*, IUnknown*, BOOL)
        {
            return S_OK;
        }

        void __stdcall release_ownership(void*)
        {

        }

        HRESULT __stdcall get_gamma_control_capabilities(void*, DXGI_GAMMA_CONTROL_CAPABILITIES*)
        {
            return DXGI_ERROR_NOT_FOUND;
        }

        HRESULT __stdcall set_gamma_control(void*, DXGI_GAMMA_CONTROL*)
        {
            return S_OK;
        }

        HRESULT __stdcall get_gamma_control(void*, DXGI_GAMMA_CONTROL*)
        {
            return DXGI_ERROR_NOT_FOUND;
        }

        HRESULT __stdcall set_display_surface(void*, IDXGISurface*)
        {
            return S_OK;
        }

        HRESULT __stdcall get_display_surface_data(void*, IDXGISurface*)
        {
            return DXGI_ERROR_NOT_FOUND;
        }

        HRESULT __stdcall get_frame_statistics(void*, DXGI_FRAME_STATISTICS*)
        {
            return DXGI_ERROR_NOT_FOUND;
        }
    }

    dxgi_output* dxgi_output::create()
    {
        static dxgi_output instance{};
        static dxgi_output::vtable vtable{};
        instance.__vftable = &vtable;

        dxgi_object::initialize(&vtable);
        vtable.get_desc = get_desc;
        vtable.get_display_mode_list = get_display_mode_list;
        vtable.find_closest_matching_mode = find_closest_matching_mode;
        vtable.wait_for_vblank = wait_for_vblank;
        vtable.take_ownership = take_ownership;
        vtable.release_ownership = release_ownership;
        vtable.get_gamma_control_capabilities = get_gamma_control_capabilities;
        vtable.set_gamma_control = set_gamma_control;
        vtable.get_gamma_control = get_gamma_control;
        vtable.set_display_surface = set_display_surface;
        vtable.get_display_surface_data = get_display_surface_data;
        vtable.get_frame_statistics = get_frame_statistics;

        return &instance;
    }

    dxgi_output* dxgi_output::get_instance()
    {
        static const auto instance = create();
        return instance;
    }
}
