#include <std_include.hpp>

#include "d3d11_class_instance.hpp"
#include "d3d11_class_linkage.hpp"

namespace directx
{
	namespace
	{
		HRESULT __stdcall get_class_instance(void*, LPCSTR, UINT, ID3D11ClassInstance** instance)
		{
			*instance = d3d11_class_instance::get<ID3D11ClassInstance>();
			return S_OK;
		}

		HRESULT __stdcall create_class_instance(void*, LPCSTR, UINT, UINT, UINT, UINT, ID3D11ClassInstance** instance)
		{
			*instance = d3d11_class_instance::get<ID3D11ClassInstance>();
			return S_OK;
		}
	}

	d3d11_class_linkage* d3d11_class_linkage::create()
	{
		static d3d11_class_linkage instance{};
		static d3d11_class_linkage::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_device_child::initialize(&vtable);

		vtable.get_class_instance = get_class_instance;
		vtable.create_class_instance = create_class_instance;
		
		return &instance;
	}
		
	d3d11_class_linkage* d3d11_class_linkage::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
