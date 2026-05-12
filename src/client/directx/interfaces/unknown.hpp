#pragma once

#include <d3d11.h>

namespace directx
{
	struct unknown 
	{
		struct vtable
		{
			HRESULT(__stdcall* query_interface)(void*, IID* riid, void** object);
			ULONG (__stdcall* add_ref)(void*);
            ULONG (__stdcall* release)(void*);
		};
	
		static void initialize(unknown::vtable*);
	};
}
