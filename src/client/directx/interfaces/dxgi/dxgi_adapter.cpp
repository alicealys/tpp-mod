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
			if (index > 1)
			{
				*output = NULL;
				return DXGI_ERROR_NOT_FOUND;
			}

			*output = dxgi_output::get<IDXGIOutput>();
			return S_OK;
		}

		HRESULT __stdcall get_desc(void*, DXGI_ADAPTER_DESC* desc)
		{
			desc->DedicatedVideoMemory = 1;
			desc->DedicatedSystemMemory = 1;
			desc->SharedSystemMemory = 1;
			return S_OK;
		}

		HRESULT __stdcall check_interface_support(void*, GUID*, LARGE_INTEGER*)
		{
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
		vtable.get_desc1 = get_desc;

		return &instance;
	}

	dxgi_adapter* dxgi_adapter::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
