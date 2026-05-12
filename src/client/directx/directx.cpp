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
		if (!game::environment::is_mgo() || !game::environment::is_eng())
		{
			return;
		}

		utils::hook::nop(0x14A16243C, 5);
		utils::hook::nop(0x14A16244A, 5);

		utils::hook::nop(0x140BBA76E, 3);
		utils::hook::jump(0x141465284, create_dxgi_factory);
		utils::hook::jump(0x14146528A, d3d11_create_device);
	}
}
