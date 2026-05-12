#pragma once

#include "d3d11_device_child.hpp"

namespace directx
{
	struct d3d11_device_context
	{
		struct vtable : d3d11_device_child::vtable
		{
			void (__stdcall* vs_set_constant_buffers)(void*, UINT, UINT, ID3D11Buffer**);
			void (__stdcall* ps_set_shader_resources)(void*, UINT, UINT, ID3D11ShaderResourceView**);
			void (__stdcall* ps_set_shader)(void*, ID3D11PixelShader*, ID3D11ClassInstance**, UINT);
			void (__stdcall* ps_set_samplers)(void*, UINT, UINT, ID3D11SamplerState**);
			void (__stdcall* vs_set_shader)(void*, ID3D11VertexShader*, ID3D11ClassInstance**, UINT);
			void (__stdcall* draw_indexed)(void*, UINT, UINT, INT);
			void (__stdcall* draw)(void*, UINT, UINT);
			HRESULT (__stdcall* map)(void*, ID3D11Resource*, UINT, D3D11_MAP, UINT, D3D11_MAPPED_SUBRESOURCE*);
			void (__stdcall* unmap)(void*, ID3D11Resource*, UINT);
			void (__stdcall* ps_set_constant_buffers)(void*, UINT, UINT, ID3D11Buffer**);
			void (__stdcall* ia_set_input_layout)(void*, ID3D11InputLayout*);
			void (__stdcall* ia_set_vertex_buffers)(void*, UINT, UINT, ID3D11Buffer**, UINT*, UINT*);
			void (__stdcall* ia_set_index_buffer)(void*, ID3D11Buffer*, DXGI_FORMAT, UINT);
			void (__stdcall* draw_indexed_instance)(void*, UINT, UINT, UINT, INT, UINT);
			void (__stdcall* draw_instanced)(void*, UINT, UINT, UINT, UINT);
			void (__stdcall* gs_set_constant_buffers)(void*, UINT, UINT, ID3D11Buffer**);
			void (__stdcall* gs_set_shader)(void*, ID3D11GeometryShader*, ID3D11ClassInstance**, UINT);
			void (__stdcall* ia_set_primitive_technology)(void*, D3D11_PRIMITIVE_TOPOLOGY);
			void (__stdcall* vs_set_shader_resources)(void*, UINT, UINT, ID3D11ShaderResourceView**);
			void (__stdcall* vs_set_samplers)(void*, UINT, UINT, ID3D11SamplerState**);
			void (__stdcall* begin)(void*, ID3D11Asynchronous*);
			void (__stdcall* end)(void*, ID3D11Asynchronous*);
			HRESULT(__stdcall* get_data)(void*, ID3D11Asynchronous*, void*, UINT, UINT);
			void (__stdcall* set_predication)(void*, ID3D11Predicate*, BOOL);
			void (__stdcall* gs_set_shader_resources)(void*, UINT, UINT, ID3D11ShaderResourceView**);
			void (__stdcall* gs_set_samplers)(void*, UINT, UINT, ID3D11SamplerState**);
			void (__stdcall* om_set_render_targets)(void*, UINT, ID3D11RenderTargetView**, ID3D11DepthStencilView*);
			void (__stdcall* om_set_render_targets_and_unordered_access_views)(void*, UINT, ID3D11RenderTargetView**, ID3D11DepthStencilView*, UINT, UINT, ID3D11UnorderedAccessView**, UINT*);
			void (__stdcall* om_set_blend_state)(void*, ID3D11BlendState*, FLOAT[4], UINT);
			void (__stdcall* om_set_depth_stencilstate)(void*, ID3D11DepthStencilState*, UINT);
			void (__stdcall* so_set_targets)(void*, UINT, ID3D11Buffer**, UINT*);
			void (__stdcall* draw_auto)(void*);
			void (__stdcall* draw_indexed_instanced_indirect)(void*, ID3D11Buffer*, UINT);
			void (__stdcall* draw_instanced_indirect)(void*, ID3D11Buffer*, UINT);
			void (__stdcall* dispatch)(void*, UINT, UINT, UINT);
			void (__stdcall* dispatch_indirect)(void*, ID3D11Buffer*, UINT);
			void (__stdcall* rs_set_state)(void*, ID3D11RasterizerState*);
			void (__stdcall* rs_set_viewports)(void*, UINT, D3D11_VIEWPORT*);
			void (__stdcall* rs_set_scissor_rects)(void*, UINT, D3D11_RECT*);
			void (__stdcall* copy_subresource_region)(void*, ID3D11Resource*, UINT, UINT, UINT, UINT, ID3D11Resource*, UINT, D3D11_BOX*);
			void (__stdcall* copy_resource)(void*, ID3D11Resource*, ID3D11Resource*);
			void (__stdcall* update_subresource)(void*, ID3D11Resource*, UINT, D3D11_BOX*, void*, UINT, UINT);
			void (__stdcall* copy_structure_count)(void*, ID3D11Buffer*, UINT, ID3D11UnorderedAccessView*);
			void (__stdcall* clear_render_targetview)(void*, ID3D11RenderTargetView*, FLOAT[4]);
			void (__stdcall* clear_unordered_access_view_uint)(void*, ID3D11UnorderedAccessView*, UINT[4]);
			void (__stdcall* clear_unordered_access_view_float)(void*, ID3D11UnorderedAccessView*, FLOAT[4]);
			void (__stdcall* clear_depth_stencil_view)(void*, ID3D11DepthStencilView*, UINT, FLOAT, UINT8);
			void (__stdcall* generate_mips)(void*, ID3D11ShaderResourceView*);
			void (__stdcall* set_resource_min_lod)(void*, ID3D11Resource*, FLOAT);
			FLOAT(__stdcall* get_resource_min_lod)(void*, ID3D11Resource*);
			void (__stdcall* resolve_subresource)(void*, ID3D11Resource*, UINT, ID3D11Resource*, UINT, DXGI_FORMAT);
			void (__stdcall* execute_command_list)(void*, ID3D11CommandList*, BOOL);
			void (__stdcall* hs_set_shader_resources)(void*, UINT, UINT, ID3D11ShaderResourceView**);
			void (__stdcall* hs_set_shader)(void*, ID3D11HullShader*, ID3D11ClassInstance**, UINT );
			void (__stdcall* hs_set_samplers)(void*, UINT, UINT, ID3D11SamplerState**);
			void (__stdcall* hs_set_constant_buffers)(void*, UINT, UINT, ID3D11Buffer**);
			void (__stdcall* ds_set_shader_resources)(void*, UINT, UINT, ID3D11ShaderResourceView**);
			void (__stdcall* ds_set_shader)(void*, ID3D11DomainShader*, ID3D11ClassInstance**, UINT);
			void (__stdcall* ds_set_samplers)(void*, UINT, UINT, ID3D11SamplerState**);
			void (__stdcall* ds_set_constant_buffers)(void*, UINT, UINT, ID3D11Buffer**);
			void (__stdcall* cs_set_shader_resources)(void*, UINT, UINT, ID3D11ShaderResourceView**);
			void (__stdcall* cs_set_unordered_access_views)(void*, UINT, UINT, ID3D11UnorderedAccessView**, UINT*);
			void (__stdcall* cs_set_shader)(void*, ID3D11ComputeShader*, ID3D11ClassInstance**, UINT);
			void (__stdcall* cs_set_samplers)(void*, UINT, UINT, ID3D11SamplerState**);
			void (__stdcall* cs_set_constant_buffers)(void*, UINT, UINT, ID3D11Buffer**);
			void (__stdcall* vs_get_constant_buffers)(void*, UINT, UINT, ID3D11Buffer**);
			void (__stdcall* ps_get_shader_resources)(void*, UINT, UINT, ID3D11ShaderResourceView**);
			void (__stdcall* ps_get_shader)(void*, ID3D11PixelShader**, ID3D11ClassInstance**, UINT*);
			void (__stdcall* ps_get_samplers)(void*, UINT, UINT, ID3D11SamplerState**);
			void (__stdcall* vs_get_shader)(void*, ID3D11VertexShader**, ID3D11ClassInstance**, UINT*);
			void (__stdcall* ps_get_constant_buffers)(void*, UINT, UINT, ID3D11Buffer**);
			void (__stdcall* ia_get_input_layout)(void*, ID3D11InputLayout**);
			void (__stdcall* ia_get_vertex_buffers)(void*, UINT, UINT, ID3D11Buffer**, UINT*, UINT*);
			void (__stdcall* ia_get_index_buffer)(void*, ID3D11Buffer**, DXGI_FORMAT*, UINT*);
			void (__stdcall* gs_get_constant_buffers)(void*, UINT, UINT, ID3D11Buffer**);
			void (__stdcall* gs_get_shader)(void*, ID3D11GeometryShader**, ID3D11ClassInstance**, UINT*);
			void (__stdcall* ia_get_primitive_topology)(void*, D3D11_PRIMITIVE_TOPOLOGY*);
			void (__stdcall* vs_get_shader_resources)(void*, UINT, UINT, ID3D11ShaderResourceView**);
			void (__stdcall* vs_get_samplers)(void*, UINT, UINT, ID3D11SamplerState**);
			void (__stdcall* get_predication)(void*, ID3D11Predicate**, BOOL*);
			void (__stdcall* gs_get_shader_resources)(void*, UINT, UINT, ID3D11ShaderResourceView**);
			void (__stdcall* gs_get_samplers)(void*, UINT, UINT, ID3D11SamplerState**);
			void (__stdcall* om_get_render_targets)(void*, UINT, ID3D11RenderTargetView**, ID3D11DepthStencilView**);
			void (__stdcall* om_get_render_targets_and_unordered_access_views)(void*, UINT, ID3D11RenderTargetView**, ID3D11DepthStencilView**, UINT, UINT, ID3D11UnorderedAccessView**);
			void (__stdcall* om_get_blend_state)(void*, ID3D11BlendState**, FLOAT[4], UINT*);
			void (__stdcall* om_get_depth_stencil_state)(void*, ID3D11DepthStencilState**, UINT*);
			void (__stdcall* so_get_targets)(void*, UINT, ID3D11Buffer**);
			void (__stdcall* rs_get_state)(void*, ID3D11RasterizerState**);
			void (__stdcall* rs_get_viewports)(void*, UINT*, D3D11_VIEWPORT*);
			void (__stdcall* rs_get_scissor_rects)(void*, UINT*, D3D11_RECT*);
			void (__stdcall* hs_get_shader_resources)(void*, UINT, UINT, ID3D11ShaderResourceView**);
			void (__stdcall* hs_get_shader)(void*, ID3D11HullShader**, ID3D11ClassInstance**, UINT*);
			void (__stdcall* hs_get_samplers)(void*, UINT, UINT, ID3D11SamplerState**);
			void (__stdcall* hs_get_constant_buffers)(void*, UINT, UINT, ID3D11Buffer**);
			void (__stdcall* ds_get_shader_resources)(void*, UINT, UINT, ID3D11ShaderResourceView**);
			void (__stdcall* ds_get_shader)(void*, ID3D11DomainShader**, ID3D11ClassInstance**, UINT*);
			void (__stdcall* ds_get_samplers)(void*, UINT, UINT, ID3D11SamplerState**);
			void (__stdcall* ds_get_constant_buffers)(void*, UINT, UINT, ID3D11Buffer**);
			void (__stdcall* cs_get_shader_resources)(void*, UINT, UINT, ID3D11ShaderResourceView**);
			void (__stdcall* cs_get_unordered_access_views)(void*, UINT, UINT, ID3D11UnorderedAccessView**);
			void (__stdcall* cs_get_shader)(void*, ID3D11ComputeShader**, ID3D11ClassInstance**, UINT*);
			void (__stdcall* cs_get_samplers)(void*, UINT, UINT, ID3D11SamplerState**);
			void (__stdcall* cs_get_constant_buffers)(void*, UINT, UINT, ID3D11Buffer**);
			void (__stdcall* clear_state)(void*);
			void (__stdcall* flush)(void*);
			D3D11_DEVICE_CONTEXT_TYPE (__stdcall* get_type)(void*);
			UINT (__stdcall* get_context_flags)(void*);
			HRESULT (__stdcall* finish_command_list)(void*, BOOL, ID3D11CommandList**);
		};
		
		static_assert(offsetof(d3d11_device_context::vtable, get_data) == 0xE8);
		static_assert(offsetof(d3d11_device_context::vtable, execute_command_list) == 0x1D0);
		static_assert(offsetof(d3d11_device_context::vtable, clear_state) == 0x370);
		static_assert(offsetof(d3d11_device_context::vtable, flush) == 0x378);

		vtable* __vftable;

		static d3d11_device_context* create();
		static d3d11_device_context* get_instance();

		template <typename T>
		static T* get()
		{
			return reinterpret_cast<T*>(get_instance());
		}

	};
}
