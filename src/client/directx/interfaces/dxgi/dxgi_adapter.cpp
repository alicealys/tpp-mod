#include <std_include.hpp>

#include "dxgi_adapter.hpp"
#include "dxgi_output.hpp"
#include "dxgi_factory.hpp"

namespace directx
{
	namespace
	{
		HRESULT __stdcall enum_outputs(void*, UINT index, IDXGIOutput** output)
		{
			if (index > 0)
			{
				return DXGI_ERROR_NOT_FOUND;
			}

			*output = dxgi_output::get<IDXGIOutput>();
			return S_OK;
		}

		HRESULT __stdcall get_desc(void*, DXGI_ADAPTER_DESC* desc)
		{
			std::memset(desc, 0, sizeof(DXGI_ADAPTER_DESC));
			desc->DedicatedVideoMemory = 1;
			desc->DedicatedSystemMemory = 1;
			desc->SharedSystemMemory = 1;
			return S_OK;
		}

		HRESULT __stdcall check_interface_support(void*, GUID*, LARGE_INTEGER*)
		{
			return S_OK;
		}

		HRESULT __stdcall get_desc1(void*, DXGI_ADAPTER_DESC1* desc)
		{
			std::memset(desc, 0, sizeof(DXGI_ADAPTER_DESC1));
			desc->VendorId = 4318;
			desc->DeviceId = 9568;
			desc->SubSysId = 175050792;
			desc->Revision = 161;
			desc->DedicatedVideoMemory = 6287261696;
			desc->DedicatedSystemMemory = 0;
			desc->SharedSystemMemory = 8241936384;
			desc->AdapterLuid.LowPart = 74175;
			desc->AdapterLuid.HighPart = 0;
			return S_OK;
		}
	}

	dxgi_adapter* dxgi_adapter::create()
	{
		static dxgi_adapter instance{};
		static dxgi_adapter::vtable vtable{};
		instance.__vftable = &vtable;

		dxgi_object::initialize(&vtable);
		vtable.enum_outputs = enum_outputs;
		vtable.get_desc = get_desc;
		vtable.check_interface_support = check_interface_support;
		vtable.get_desc1 = get_desc1;

		return &instance;
	}

	dxgi_adapter* dxgi_adapter::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
