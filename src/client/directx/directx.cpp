#include <std_include.hpp>

#include "game/game.hpp"

#include "directx.hpp"

#include <utils/hook.hpp>

namespace directx
{
	namespace
	{
		HRESULT __stdcall create_dxgi_factory(IID* riid, void** factory)
		{
			*factory = dxgi_factory::get_instance();
			return S_OK;
		}

		HRESULT __stdcall d3d11_create_device(IDXGIAdapter* adapter, D3D_DRIVER_TYPE driver_type, HMODULE software, UINT flags,
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

		utils::hook::nop(0x140BBC9D0, 5);
		utils::hook::nop(0x140BBC9DE, 5);
		utils::hook::nop(0x140BB9BBE, 3);

		utils::hook::jump(0x141464A94, create_dxgi_factory);
		utils::hook::jump(0x141464A9A, d3d11_create_device);
	}
}
