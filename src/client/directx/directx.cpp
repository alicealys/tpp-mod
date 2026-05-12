#include <std_include.hpp>

#include "game/game.hpp"

#include "directx.hpp"

#include <utils/hook.hpp>

namespace directx
{
	namespace
	{
		HRESULT create_dxgi_factory(const IID& riid, void** factory)
		{
			*factory = dxgi_factory::get_instance();
			return S_OK;
		}

		HRESULT d3d11_create_device(IDXGIAdapter* adapter, D3D_DRIVER_TYPE driver_type, HMODULE software, UINT flags,
			D3D_FEATURE_LEVEL* p_feature_levels, UINT feature_levels, UINT sdk_version,
			ID3D11Device** device, D3D_FEATURE_LEVEL* p_feature_level, ID3D11DeviceContext** immediate_context)
		{
			*p_feature_level = D3D_FEATURE_LEVEL_11_0;
			*device = d3d11_device::get<ID3D11Device>();
			*immediate_context = d3d11_device_context::get<ID3D11DeviceContext>();
			return S_OK;
		}
	}

	void disable_d3d11()
	{
		utils::hook::nop(SELECT_VALUE(0x1402BA88E, 0x140BBA76E, 0x1402BA32E, 0x140BB9DBE), 3);
		utils::hook::jump(SELECT_VALUE(0x141A08EA4, 0x141465284, 0x141A08FC4, 0x141465124), create_dxgi_factory);
		utils::hook::jump(SELECT_VALUE(0x141A08EAA, 0x14146528A, 0x141A08FCA, 0x14146512A), d3d11_create_device);
	}
}
