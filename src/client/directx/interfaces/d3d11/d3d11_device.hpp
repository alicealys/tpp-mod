#pragma once

#include "d3d11_device_child.hpp"

namespace directx
{
	struct d3d11_device
	{
		struct vtable : unknown::vtable
		{
			HRESULT(__stdcall* create_buffer)(void*, D3D11_BUFFER_DESC*, D3D11_SUBRESOURCE_DATA*, ID3D11Buffer**);
			HRESULT(__stdcall* create_texture_1d)(void*, D3D11_TEXTURE1D_DESC*, D3D11_SUBRESOURCE_DATA*, ID3D11Texture1D**);
			HRESULT(__stdcall* create_texture_2d)(void*, D3D11_TEXTURE2D_DESC*, D3D11_SUBRESOURCE_DATA*, ID3D11Texture2D**);
			HRESULT(__stdcall* create_texture_3d)(void*, D3D11_TEXTURE3D_DESC*, D3D11_SUBRESOURCE_DATA*, ID3D11Texture3D**);
			HRESULT(__stdcall* create_shader_resource_view)(void*, ID3D11Resource*, D3D11_SHADER_RESOURCE_VIEW_DESC*, ID3D11ShaderResourceView**);
			HRESULT(__stdcall* create_unordered_access_view)(void*, ID3D11Resource*, D3D11_UNORDERED_ACCESS_VIEW_DESC*, ID3D11UnorderedAccessView**);
			HRESULT(__stdcall* create_render_target_view)(void*, ID3D11Resource*, D3D11_RENDER_TARGET_VIEW_DESC*, ID3D11RenderTargetView**);
			HRESULT(__stdcall* create_depth_stencil_view)(void*, ID3D11Resource*, D3D11_DEPTH_STENCIL_VIEW_DESC*, ID3D11DepthStencilView**);
			HRESULT(__stdcall* create_input_layout)(void*, D3D11_INPUT_ELEMENT_DESC*, UINT, void*, SIZE_T, ID3D11InputLayout**);
			HRESULT(__stdcall* create_vertex_shader)(void*, void*, SIZE_T, ID3D11ClassLinkage*, ID3D11VertexShader**);
			HRESULT(__stdcall* create_geometry_shader)(void*, void*, SIZE_T, ID3D11ClassLinkage*, ID3D11GeometryShader**);
			HRESULT(__stdcall* create_geometry_shader_with_stream_output)(void*, void*, SIZE_T, D3D11_SO_DECLARATION_ENTRY*, UINT, UINT*, UINT, UINT, ID3D11ClassLinkage*, ID3D11GeometryShader**);
			HRESULT(__stdcall* create_pixel_shader)(void*, void*, SIZE_T, ID3D11ClassLinkage*, ID3D11PixelShader**);
			HRESULT(__stdcall* create_hull_shader)(void*, void*, SIZE_T, ID3D11ClassLinkage*, ID3D11HullShader**);
			HRESULT(__stdcall* create_domain_shader)(void*, void*, SIZE_T, ID3D11ClassLinkage*, ID3D11DomainShader**);
			HRESULT(__stdcall* create_compute_shader)(void*, void*, SIZE_T, ID3D11ClassLinkage*, ID3D11ComputeShader**);
			HRESULT(__stdcall* create_class_linkage)(void*, ID3D11ClassLinkage**);
			HRESULT(__stdcall* create_blend_state)(void*, D3D11_BLEND_DESC*, ID3D11BlendState**);
			HRESULT(__stdcall* create_depth_stencil_state)(void*, D3D11_DEPTH_STENCIL_DESC*, ID3D11DepthStencilState**);
			HRESULT(__stdcall* create_rasterizer_state)(void*, D3D11_RASTERIZER_DESC*, ID3D11RasterizerState**);
			HRESULT(__stdcall* create_sampler_state)(void*, D3D11_SAMPLER_DESC*, ID3D11SamplerState**);
			HRESULT(__stdcall* create_query)(void*, D3D11_QUERY_DESC*, ID3D11Query**);
			HRESULT(__stdcall* create_predicate)(void*, D3D11_QUERY_DESC*, ID3D11Predicate**);
			HRESULT(__stdcall* create_counter)(void*, D3D11_COUNTER_DESC*, ID3D11Counter**);
			HRESULT(__stdcall* create_deferred_context)(void*, UINT, ID3D11DeviceContext**);
			HRESULT(__stdcall* open_shared_resource)(void*, HANDLE, IID*, void**);
			HRESULT(__stdcall* check_format_support)(void*, DXGI_FORMAT, UINT*);
			HRESULT(__stdcall* check_multisample_quality_levels)(void*, DXGI_FORMAT, UINT, UINT*);
			void(__stdcall* check_counter_info)(void*, D3D11_COUNTER_INFO*);
			HRESULT(__stdcall* check_counter)(void*, D3D11_COUNTER_DESC*, D3D11_COUNTER_TYPE*, UINT*, LPSTR, UINT*, LPSTR, UINT*, LPSTR, UINT*);
			HRESULT(__stdcall* check_feature_support)(void*, D3D11_FEATURE, void*, UINT);
			HRESULT(__stdcall* get_private_data)(void*, GUID*, UINT*, void*);
			HRESULT(__stdcall* set_private_data)(void*, GUID*, UINT, void*);
			HRESULT(__stdcall* set_private_data_interface)(void*, GUID*, IUnknown*);
			D3D_FEATURE_LEVEL(__stdcall* get_feature_level)(void*);
			UINT(__stdcall* get_creation_flags)(void*);
			HRESULT(__stdcall* get_device_removed_reason)(void*);
			void(__stdcall* get_immediate_context)(void*, ID3D11DeviceContext**);
			HRESULT(__stdcall* set_exception_mode)(void*, UINT);
			UINT(__stdcall* get_exception_mode)(void*);
		};

		vtable* __vftable;

		static d3d11_device* create();
		static d3d11_device* get_instance();

		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}
	};
}
