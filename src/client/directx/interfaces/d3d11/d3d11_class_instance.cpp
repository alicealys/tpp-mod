#include <std_include.hpp>

#include "d3d11_class_linkage.hpp"
#include "d3d11_class_instance.hpp"

namespace directx
{
	namespace
	{
		void __stdcall get_class_linkage(void*, ID3D11ClassLinkage** linkage)
		{
			*linkage = d3d11_class_linkage::get<ID3D11ClassLinkage>();
		}

		void __stdcall get_desc(void*, D3D11_CLASS_INSTANCE_DESC* desc)
		{

		}

		void __stdcall get_instance_name(void*, LPSTR, SIZE_T* length)
		{
			*length = 0;
		}

		void __stdcall get_type_name(void*, LPSTR, SIZE_T* length)
		{
			*length = 0;
		}
	}

	d3d11_class_instance* d3d11_class_instance::create()
	{
		static d3d11_class_instance instance{};
		static d3d11_class_instance::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_device_child::initialize(&vtable);

		vtable.get_class_linkage = get_class_linkage;
		vtable.get_desc = get_desc;
		vtable.get_instance_name = get_instance_name;
		vtable.get_type_name = get_type_name;
		
		return &instance;
	}
		
	d3d11_class_instance* d3d11_class_instance::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
