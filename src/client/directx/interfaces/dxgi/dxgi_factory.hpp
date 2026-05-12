#pragma once

#include "dxgi_object.hpp"

namespace directx
{
	struct dxgi_factory
	{
		struct vtable : dxgi_object::vtable
		{
			HRESULT (__stdcall* enum_adapters)(void*, UINT, IDXGIAdapter**);
			HRESULT (__stdcall* make_window_association)(void*, HWND, UINT);
			HRESULT (__stdcall* get_window_association)(void*, HWND*);
			HRESULT (__stdcall* create_swap_chain)(void*, IUnknown*, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**);
			HRESULT (__stdcall* create_software_adapter)(void*, HMODULE, IDXGIAdapter**);
			HRESULT (__stdcall* enum_adapters1)(void*, UINT, IDXGIAdapter**);
			BOOL (__stdcall* is_current)(void*);
		};

		vtable* __vftable;

		static dxgi_factory* create();
		static dxgi_factory* get_instance();

		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	};
}
