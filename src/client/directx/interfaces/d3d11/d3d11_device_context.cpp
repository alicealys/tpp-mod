#include <std_include.hpp>

#include "d3d11_device_context.hpp"

namespace directx
{
	namespace
	{
		void __stdcall vs_set_constant_buffers(void*, UINT, UINT, ID3D11Buffer**)
		{

		}

		void __stdcall ps_set_shader_resources(void*, UINT, UINT, ID3D11ShaderResourceView**)
		{

		}

		void __stdcall ps_set_shader(void*, ID3D11PixelShader*, ID3D11ClassInstance**, UINT)
		{

		}

		void __stdcall ps_set_samplers(void*, UINT, UINT, ID3D11SamplerState**)
		{

		}

		void __stdcall vs_set_shader(void*, ID3D11VertexShader*, ID3D11ClassInstance**, UINT)
		{

		}

		void __stdcall draw_indexed(void*, UINT, UINT, INT)
		{

		}

		void __stdcall draw(void*, UINT, UINT)
		{

		}

		HRESULT __stdcall map(void*, ID3D11Resource*, UINT, D3D11_MAP, UINT, D3D11_MAPPED_SUBRESOURCE*)
		{
			return S_OK;
		}

		void __stdcall unmap(void*, ID3D11Resource*, UINT)
		{

		}

		void __stdcall ps_set_constant_buffers(void*, UINT, UINT, ID3D11Buffer**)
		{

		}

		void __stdcall ia_set_input_layout(void*, ID3D11InputLayout*)
		{

		}

		void __stdcall ia_set_vertex_buffers(void*, UINT, UINT, ID3D11Buffer**, UINT*, UINT*)
		{

		}

		void __stdcall ia_set_index_buffer(void*, ID3D11Buffer*, DXGI_FORMAT, UINT)
		{

		}

		void __stdcall draw_indexed_instance(void*, UINT, UINT, UINT, INT, UINT)
		{

		}

		void __stdcall draw_instanced(void*, UINT, UINT, UINT, UINT)
		{

		}

		void __stdcall gs_set_constant_buffers(void*, UINT, UINT, ID3D11Buffer**)
		{

		}

		void __stdcall gs_set_shader(void*, ID3D11GeometryShader*, ID3D11ClassInstance**, UINT)
		{

		}

		void __stdcall ia_set_primitive_technology(void*, D3D11_PRIMITIVE_TOPOLOGY)
		{

		}

		void __stdcall vs_set_shader_resources(void*, UINT, UINT, ID3D11ShaderResourceView**)
		{

		}

		void __stdcall vs_set_samplers(void*, UINT, UINT, ID3D11SamplerState**)
		{

		}

		void __stdcall begin(void*, ID3D11Asynchronous*)
		{

		}

		void __stdcall end(void*, ID3D11Asynchronous*)
		{

		}

		HRESULT __stdcall get_data(void*, ID3D11Asynchronous*, void* data, UINT size, UINT)
		{
			for (auto i = 0u; i < size; i++)
			{
				reinterpret_cast<unsigned char*>(data)[i] = 0xFF;
			}

			return 0;
		}

		void __stdcall set_predication(void*, ID3D11Predicate*, BOOL)
		{

		}

		void __stdcall gs_set_shader_resources(void*, UINT, UINT, ID3D11ShaderResourceView**)
		{

		}

		void __stdcall gs_set_samplers(void*, UINT, UINT, ID3D11SamplerState**)
		{

		}

		void __stdcall om_set_render_targets(void*, UINT, ID3D11RenderTargetView**, ID3D11DepthStencilView*)
		{

		}

		void __stdcall om_set_render_targets_and_unordered_access_views(void*, UINT, ID3D11RenderTargetView**, ID3D11DepthStencilView*, UINT, UINT, ID3D11UnorderedAccessView**, UINT*)
		{

		}

		void __stdcall om_set_blend_state(void*, ID3D11BlendState*, FLOAT[4], UINT)
		{

		}

		void __stdcall om_set_depth_stencilstate(void*, ID3D11DepthStencilState*, UINT)
		{

		}

		void __stdcall so_set_targets(void*, UINT, ID3D11Buffer**, UINT*)
		{

		}

		void __stdcall draw_auto(void*)
		{

		}

		void __stdcall draw_indexed_instanced_indirect(void*, ID3D11Buffer*, UINT)
		{

		}

		void __stdcall draw_instanced_indirect(void*, ID3D11Buffer*, UINT)
		{

		}

		void __stdcall dispatch(void*, UINT, UINT, UINT)
		{

		}

		void __stdcall dispatch_indirect(void*, ID3D11Buffer*, UINT)
		{

		}

		void __stdcall rs_set_state(void*, ID3D11RasterizerState*)
		{

		}

		void __stdcall rs_set_viewports(void*, UINT, D3D11_VIEWPORT*)
		{

		}

		void __stdcall rs_set_scissor_rects(void*, UINT, D3D11_RECT*)
		{

		}

		void __stdcall copy_subresource_region(void*, ID3D11Resource*, UINT, UINT, UINT, UINT, ID3D11Resource*, UINT, D3D11_BOX*)
		{

		}

		void __stdcall copy_resource(void*, ID3D11Resource*, ID3D11Resource*)
		{

		}

		void __stdcall update_subresource(void*, ID3D11Resource*, UINT, D3D11_BOX*, void*, UINT, UINT)
		{

		}

		void __stdcall copy_structure_count(void*, ID3D11Buffer*, UINT, ID3D11UnorderedAccessView*)
		{

		}

		void __stdcall clear_render_targetview(void*, ID3D11RenderTargetView*, FLOAT[4])
		{

		}

		void __stdcall clear_unordered_access_view_uint(void*, ID3D11UnorderedAccessView*, UINT[4])
		{

		}

		void __stdcall clear_unordered_access_view_float(void*, ID3D11UnorderedAccessView*, FLOAT[4])
		{

		}

		void __stdcall clear_depth_stencil_view(void*, ID3D11DepthStencilView*, UINT, FLOAT, UINT8)
		{

		}

		void __stdcall generate_mips(void*, ID3D11ShaderResourceView*)
		{

		}

		void __stdcall set_resource_min_lod(void*, ID3D11Resource*, FLOAT)
		{

		}

		FLOAT __stdcall get_resource_min_lod(void*, ID3D11Resource*)
		{
			return 0.f;
		}

		void __stdcall resolve_subresource(void*, ID3D11Resource*, UINT, ID3D11Resource*, UINT, DXGI_FORMAT)
		{

		}

		void __stdcall execute_command_list(void*, ID3D11CommandList* list, BOOL)
		{

		}

		void __stdcall hs_set_shader_resources(void*, UINT, UINT, ID3D11ShaderResourceView**)
		{

		}

		void __stdcall hs_set_shader(void*, ID3D11HullShader*, ID3D11ClassInstance**, UINT)
		{

		}

		void __stdcall hs_set_samplers(void*, UINT, UINT, ID3D11SamplerState**)
		{

		}

		void __stdcall hs_set_constant_buffers(void*, UINT, UINT, ID3D11Buffer**)
		{

		}

		void __stdcall ds_set_shader_resources(void*, UINT, UINT, ID3D11ShaderResourceView**)
		{

		}

		void __stdcall ds_set_shader(void*, ID3D11DomainShader*, ID3D11ClassInstance**, UINT)
		{

		}

		void __stdcall ds_set_samplers(void*, UINT, UINT, ID3D11SamplerState**)
		{

		}

		void __stdcall ds_set_constant_buffers(void*, UINT, UINT, ID3D11Buffer**)
		{

		}

		void __stdcall cs_set_shader_resources(void*, UINT, UINT, ID3D11ShaderResourceView**)
		{

		}

		void __stdcall cs_set_unordered_access_views(void*, UINT, UINT, ID3D11UnorderedAccessView**, UINT*)
		{

		}

		void __stdcall cs_set_shader(void*, ID3D11ComputeShader*, ID3D11ClassInstance**, UINT)
		{

		}

		void __stdcall cs_set_samplers(void*, UINT, UINT, ID3D11SamplerState**)
		{

		}

		void __stdcall cs_set_constant_buffers(void*, UINT, UINT, ID3D11Buffer**)
		{

		}

		void __stdcall vs_get_constant_buffers(void*, UINT, UINT, ID3D11Buffer**)
		{

		}

		void __stdcall ps_get_shader_resources(void*, UINT, UINT, ID3D11ShaderResourceView**)
		{

		}

		void __stdcall ps_get_shader(void*, ID3D11PixelShader**, ID3D11ClassInstance**, UINT*)
		{

		}

		void __stdcall ps_get_samplers(void*, UINT, UINT, ID3D11SamplerState**)
		{

		}

		void __stdcall vs_get_shader(void*, ID3D11VertexShader**, ID3D11ClassInstance**, UINT*)
		{

		}

		void __stdcall ps_get_constant_buffers(void*, UINT, UINT, ID3D11Buffer**)
		{

		}

		void __stdcall ia_get_input_layout(void*, ID3D11InputLayout**)
		{

		}

		void __stdcall ia_get_vertex_buffers(void*, UINT, UINT, ID3D11Buffer**, UINT*, UINT*)
		{

		}

		void __stdcall ia_get_index_buffer(void*, ID3D11Buffer**, DXGI_FORMAT*, UINT*)
		{

		}

		void __stdcall gs_get_constant_buffers(void*, UINT, UINT, ID3D11Buffer**)
		{

		}

		void __stdcall gs_get_shader(void*, ID3D11GeometryShader**, ID3D11ClassInstance**, UINT*)
		{

		}

		void __stdcall ia_get_primitive_topology(void*, D3D11_PRIMITIVE_TOPOLOGY*)
		{

		}

		void __stdcall vs_get_shader_resources(void*, UINT, UINT, ID3D11ShaderResourceView**)
		{

		}

		void __stdcall vs_get_samplers(void*, UINT, UINT, ID3D11SamplerState**)
		{

		}

		void __stdcall get_predication(void*, ID3D11Predicate**, BOOL*)
		{

		}

		void __stdcall gs_get_shader_resources(void*, UINT, UINT, ID3D11ShaderResourceView**)
		{

		}

		void __stdcall gs_get_samplers(void*, UINT, UINT, ID3D11SamplerState**)
		{

		}

		void __stdcall om_get_render_targets(void*, UINT, ID3D11RenderTargetView**, ID3D11DepthStencilView**)
		{

		}

		void __stdcall om_get_render_targets_and_unordered_access_views(void*, UINT, ID3D11RenderTargetView**, ID3D11DepthStencilView**, UINT, UINT, ID3D11UnorderedAccessView**)
		{

		}

		void __stdcall om_get_blend_state(void*, ID3D11BlendState**, FLOAT[4], UINT*)
		{

		}

		void __stdcall om_get_depth_stencil_state(void*, ID3D11DepthStencilState**, UINT*)
		{

		}

		void __stdcall so_get_targets(void*, UINT, ID3D11Buffer**)
		{

		}

		void __stdcall rs_get_state(void*, ID3D11RasterizerState**)
		{

		}

		void __stdcall rs_get_viewports(void*, UINT*, D3D11_VIEWPORT*)
		{

		}

		void __stdcall rs_get_scissor_rects(void*, UINT*, D3D11_RECT*)
		{

		}

		void __stdcall hs_get_shader_resources(void*, UINT, UINT, ID3D11ShaderResourceView**)
		{

		}

		void __stdcall hs_get_shader(void*, ID3D11HullShader**, ID3D11ClassInstance**, UINT*)
		{

		}

		void __stdcall hs_get_samplers(void*, UINT, UINT, ID3D11SamplerState**)
		{

		}

		void __stdcall hs_get_constant_buffers(void*, UINT, UINT, ID3D11Buffer**)
		{

		}

		void __stdcall ds_get_shader_resources(void*, UINT, UINT, ID3D11ShaderResourceView**)
		{

		}

		void __stdcall ds_get_shader(void*, ID3D11DomainShader**, ID3D11ClassInstance**, UINT*)
		{

		}

		void __stdcall ds_get_samplers(void*, UINT, UINT, ID3D11SamplerState**)
		{

		}

		void __stdcall ds_get_constant_buffers(void*, UINT, UINT, ID3D11Buffer**)
		{

		}

		void __stdcall cs_get_shader_resources(void*, UINT, UINT, ID3D11ShaderResourceView**)
		{

		}

		void __stdcall cs_get_unordered_access_views(void*, UINT, UINT, ID3D11UnorderedAccessView**)
		{

		}

		void __stdcall cs_get_shader(void*, ID3D11ComputeShader**, ID3D11ClassInstance**, UINT*)
		{

		}

		void __stdcall cs_get_samplers(void*, UINT, UINT, ID3D11SamplerState**)
		{

		}

		void __stdcall cs_get_constant_buffers(void*, UINT, UINT, ID3D11Buffer**)
		{

		}

		void __stdcall clear_state(void*)
		{

		}

		void __stdcall flush(void*)
		{

		}

		D3D11_DEVICE_CONTEXT_TYPE __stdcall get_type(void*)
		{
			return D3D11_DEVICE_CONTEXT_IMMEDIATE;
		}

		UINT __stdcall get_context_flags(void*)
		{
			return 0;
		}

		HRESULT __stdcall finish_command_list(void*, BOOL, ID3D11CommandList**)
		{
			return S_OK;
		}
	}

	d3d11_device_context* d3d11_device_context::create()
	{
		static d3d11_device_context instance{};
		static d3d11_device_context::vtable vtable{};
		instance.__vftable = &vtable;

		d3d11_device_child::initialize(&vtable);

		vtable.vs_set_constant_buffers = vs_set_constant_buffers;
		vtable.ps_set_shader_resources = ps_set_shader_resources;
		vtable.ps_set_shader = ps_set_shader;
		vtable.ps_set_samplers = ps_set_samplers;
		vtable.vs_set_shader = vs_set_shader;
		vtable.draw_indexed = draw_indexed;
		vtable.draw = draw;
		vtable.map = map;
		vtable.unmap = unmap;
		vtable.ps_set_constant_buffers = ps_set_constant_buffers;
		vtable.ia_set_input_layout = ia_set_input_layout;
		vtable.ia_set_vertex_buffers = ia_set_vertex_buffers;
		vtable.ia_set_index_buffer = ia_set_index_buffer;
		vtable.draw_indexed_instance = draw_indexed_instance;
		vtable.draw_instanced = draw_instanced;
		vtable.gs_set_constant_buffers = gs_set_constant_buffers;
		vtable.gs_set_shader = gs_set_shader;
		vtable.ia_set_primitive_technology = ia_set_primitive_technology;
		vtable.vs_set_shader_resources = vs_set_shader_resources;
		vtable.vs_set_samplers = vs_set_samplers;
		vtable.begin = begin;
		vtable.end = end;
		vtable.get_data = get_data;
		vtable.set_predication = set_predication;
		vtable.gs_set_shader_resources = gs_set_shader_resources;
		vtable.gs_set_samplers = gs_set_samplers;
		vtable.om_set_render_targets = om_set_render_targets;
		vtable.om_set_render_targets_and_unordered_access_views = om_set_render_targets_and_unordered_access_views;
		vtable.om_set_blend_state = om_set_blend_state;
		vtable.om_set_depth_stencilstate = om_set_depth_stencilstate;
		vtable.so_set_targets = so_set_targets;
		vtable.draw_auto = draw_auto;
		vtable.draw_indexed_instanced_indirect = draw_indexed_instanced_indirect;
		vtable.draw_instanced_indirect = draw_instanced_indirect;
		vtable.dispatch = dispatch;
		vtable.dispatch_indirect = dispatch_indirect;
		vtable.rs_set_state = rs_set_state;
		vtable.rs_set_viewports = rs_set_viewports;
		vtable.rs_set_scissor_rects = rs_set_scissor_rects;
		vtable.copy_subresource_region = copy_subresource_region;
		vtable.copy_resource = copy_resource;
		vtable.update_subresource = update_subresource;
		vtable.copy_structure_count = copy_structure_count;
		vtable.clear_render_targetview = clear_render_targetview;
		vtable.clear_unordered_access_view_uint = clear_unordered_access_view_uint;
		vtable.clear_unordered_access_view_float = clear_unordered_access_view_float;
		vtable.clear_depth_stencil_view = clear_depth_stencil_view;
		vtable.generate_mips = generate_mips;
		vtable.set_resource_min_lod = set_resource_min_lod;
		vtable.get_resource_min_lod = get_resource_min_lod;
		vtable.resolve_subresource = resolve_subresource;
		vtable.execute_command_list = execute_command_list;
		vtable.hs_set_shader_resources = hs_set_shader_resources;
		vtable.hs_set_shader = hs_set_shader;
		vtable.hs_set_samplers = hs_set_samplers;
		vtable.hs_set_constant_buffers = hs_set_constant_buffers;
		vtable.ds_set_shader_resources = ds_set_shader_resources;
		vtable.ds_set_shader = ds_set_shader;
		vtable.ds_set_samplers = ds_set_samplers;
		vtable.ds_set_constant_buffers = ds_set_constant_buffers;
		vtable.cs_set_shader_resources = cs_set_shader_resources;
		vtable.cs_set_unordered_access_views = cs_set_unordered_access_views;
		vtable.cs_set_shader = cs_set_shader;
		vtable.cs_set_samplers = cs_set_samplers;
		vtable.cs_set_constant_buffers = cs_set_constant_buffers;
		vtable.vs_get_constant_buffers = vs_get_constant_buffers;
		vtable.ps_get_shader_resources = ps_get_shader_resources;
		vtable.ps_get_shader = ps_get_shader;
		vtable.ps_get_samplers = ps_get_samplers;
		vtable.vs_get_shader = vs_get_shader;
		vtable.ps_get_constant_buffers = ps_get_constant_buffers;
		vtable.ia_get_input_layout = ia_get_input_layout;
		vtable.ia_get_vertex_buffers = ia_get_vertex_buffers;
		vtable.ia_get_index_buffer = ia_get_index_buffer;
		vtable.gs_get_constant_buffers = gs_get_constant_buffers;
		vtable.gs_get_shader = gs_get_shader;
		vtable.ia_get_primitive_topology = ia_get_primitive_topology;
		vtable.vs_get_shader_resources = vs_get_shader_resources;
		vtable.vs_get_samplers = vs_get_samplers;
		vtable.get_predication = get_predication;
		vtable.gs_get_shader_resources = gs_get_shader_resources;
		vtable.gs_get_samplers = gs_get_samplers;
		vtable.om_get_render_targets = om_get_render_targets;
		vtable.om_get_render_targets_and_unordered_access_views = om_get_render_targets_and_unordered_access_views;
		vtable.om_get_blend_state = om_get_blend_state;
		vtable.om_get_depth_stencil_state = om_get_depth_stencil_state;
		vtable.so_get_targets = so_get_targets;
		vtable.rs_get_state = rs_get_state;
		vtable.rs_get_viewports = rs_get_viewports;
		vtable.rs_get_scissor_rects = rs_get_scissor_rects;
		vtable.hs_get_shader_resources = hs_get_shader_resources;
		vtable.hs_get_shader = hs_get_shader;
		vtable.hs_get_samplers = hs_get_samplers;
		vtable.hs_get_constant_buffers = hs_get_constant_buffers;
		vtable.ds_get_shader_resources = ds_get_shader_resources;
		vtable.ds_get_shader = ds_get_shader;
		vtable.ds_get_samplers = ds_get_samplers;
		vtable.ds_get_constant_buffers = ds_get_constant_buffers;
		vtable.cs_get_shader_resources = cs_get_shader_resources;
		vtable.cs_get_unordered_access_views = cs_get_unordered_access_views;
		vtable.cs_get_shader = cs_get_shader;
		vtable.cs_get_samplers = cs_get_samplers;
		vtable.cs_get_constant_buffers = cs_get_constant_buffers;
		vtable.clear_state = clear_state;
		vtable.flush = flush;
		vtable.get_type = get_type;
		vtable.get_context_flags = get_context_flags;
		vtable.finish_command_list = finish_command_list;

		return &instance;
	}

	d3d11_device_context* d3d11_device_context::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
