#include <std_include.hpp>

#include "../dxgi/dxgi_adapter.hpp"
#include "../dxgi/dxgi_device.hpp"

#include "d3d11_blend_state.hpp"
#include "d3d11_buffer.hpp"
#include "d3d11_class_linkage.hpp"
#include "d3d11_compute_shader.hpp"
#include "d3d11_counter.hpp"
#include "d3d11_depth_stencil_state.hpp"
#include "d3d11_depth_stencil_view.hpp"
#include "d3d11_device.hpp"
#include "d3d11_device_context.hpp"
#include "d3d11_domain_shader.hpp"
#include "d3d11_geometry_shader.hpp"
#include "d3d11_hull_shader.hpp"
#include "d3d11_input_layout.hpp"
#include "d3d11_pixel_shader.hpp"
#include "d3d11_predicate.hpp"
#include "d3d11_query.hpp"
#include "d3d11_rasterizer_state.hpp"
#include "d3d11_render_target_view.hpp"
#include "d3d11_sampler_state.hpp"
#include "d3d11_shader_resource_view.hpp"
#include "d3d11_texture1d.hpp"
#include "d3d11_texture2d.hpp"
#include "d3d11_texture3d.hpp"
#include "d3d11_unordered_access_view.hpp"
#include "d3d11_vertex_shader.hpp"
#include <utils/hook.hpp>

namespace directx
{
	namespace
	{
		HRESULT __stdcall create_buffer(void*, D3D11_BUFFER_DESC*, D3D11_SUBRESOURCE_DATA*, ID3D11Buffer** resource)
		{
			*resource = d3d11_buffer::get<ID3D11Buffer>();
			return S_OK;
		}

		HRESULT __stdcall create_texture_1d(void*, D3D11_TEXTURE1D_DESC*, D3D11_SUBRESOURCE_DATA*, ID3D11Texture1D** resource)
		{
			*resource = d3d11_texture1d::get<ID3D11Texture1D>();
			return S_OK;
		}

		HRESULT __stdcall create_texture_2d(void*, D3D11_TEXTURE2D_DESC*, D3D11_SUBRESOURCE_DATA*, ID3D11Texture2D** resource)
		{
			*resource = d3d11_texture2d::get<ID3D11Texture2D>();
			return S_OK;
		}

		HRESULT __stdcall create_texture_3d(void*, D3D11_TEXTURE3D_DESC*, D3D11_SUBRESOURCE_DATA*, ID3D11Texture3D** resource)
		{
			*resource = d3d11_texture3d::get<ID3D11Texture3D>();
			return S_OK;
		}

		HRESULT __stdcall create_shader_resource_view(void*, ID3D11Resource*, D3D11_SHADER_RESOURCE_VIEW_DESC*, ID3D11ShaderResourceView** resource)
		{
			*resource = d3d11_shader_resource_view::get<ID3D11ShaderResourceView>();
			return S_OK;
		}

		HRESULT __stdcall create_unordered_access_view(void*, ID3D11Resource*, D3D11_UNORDERED_ACCESS_VIEW_DESC*, ID3D11UnorderedAccessView** resource)
		{
			*resource = d3d11_unordered_access_view::get<ID3D11UnorderedAccessView>();
			return S_OK;
		}

		HRESULT __stdcall create_render_target_view(void*, ID3D11Resource*, D3D11_RENDER_TARGET_VIEW_DESC*, ID3D11RenderTargetView** resource)
		{
			*resource = d3d11_render_target_view::get<ID3D11RenderTargetView>();
			return S_OK;
		}

		HRESULT __stdcall create_depth_stencil_view(void*, ID3D11Resource*, D3D11_DEPTH_STENCIL_VIEW_DESC*, ID3D11DepthStencilView** resource)
		{
			*resource = d3d11_depth_stencil_view::get<ID3D11DepthStencilView>();
			return S_OK;
		}

		HRESULT __stdcall create_input_layout(void*, D3D11_INPUT_ELEMENT_DESC*, UINT, void*, SIZE_T, ID3D11InputLayout** resource)
		{
			*resource = d3d11_input_layout::get<ID3D11InputLayout>();
			return S_OK;
		}

		HRESULT __stdcall create_vertex_shader(void*, void*, SIZE_T, ID3D11ClassLinkage*, ID3D11VertexShader** resource )
		{
			*resource = d3d11_vertex_shader::get<ID3D11VertexShader>();
			return S_OK;
		}

		HRESULT __stdcall create_geometry_shader(void*, void*, SIZE_T, ID3D11ClassLinkage*, ID3D11GeometryShader** resource)
		{
			*resource = d3d11_geometry_shader::get<ID3D11GeometryShader>();
			return S_OK;
		}

		HRESULT __stdcall create_geometry_shader_with_stream_output(void*, void*, SIZE_T, D3D11_SO_DECLARATION_ENTRY*, UINT, UINT*, UINT, UINT, ID3D11ClassLinkage*, ID3D11GeometryShader** resource)
		{
			*resource = d3d11_geometry_shader::get<ID3D11GeometryShader>();
			return S_OK;
		}

		HRESULT __stdcall create_pixel_shader(void*, void*, SIZE_T, ID3D11ClassLinkage*, ID3D11PixelShader** resource)
		{
			*resource = d3d11_pixel_shader::get<ID3D11PixelShader>();
			return S_OK;
		}

		HRESULT __stdcall create_hull_shader(void*, void*, SIZE_T, ID3D11ClassLinkage*, ID3D11HullShader** resource)
		{
			*resource = d3d11_hull_shader::get<ID3D11HullShader>();
			return S_OK;
		}

		HRESULT __stdcall create_domain_shader(void*, void*, SIZE_T, ID3D11ClassLinkage*, ID3D11DomainShader** resource)
		{
			*resource = d3d11_domain_shader::get<ID3D11DomainShader>();
			return S_OK;
		}

		HRESULT __stdcall create_compute_shader(void*, void*, SIZE_T, ID3D11ClassLinkage*, ID3D11ComputeShader** resource)
		{
			*resource = d3d11_compute_shader::get<ID3D11ComputeShader>();
			return S_OK;
		}

		HRESULT __stdcall create_class_linkage(void*, ID3D11ClassLinkage** resource)
		{
			*resource = d3d11_class_linkage::get<ID3D11ClassLinkage>();
			return S_OK;
		}

		HRESULT __stdcall create_blend_state(void*, D3D11_BLEND_DESC*, ID3D11BlendState** resource)
		{
			*resource = d3d11_blend_state::get<ID3D11BlendState>();
			return S_OK;
		}

		HRESULT __stdcall create_depth_stencil_state(void*, D3D11_DEPTH_STENCIL_DESC*, ID3D11DepthStencilState** resource)
		{
			*resource = d3d11_depth_stencil_state::get<ID3D11DepthStencilState>();
			return S_OK;
		}

		HRESULT __stdcall create_rasterizer_state(void*, D3D11_RASTERIZER_DESC*, ID3D11RasterizerState** resource)
		{
			*resource = d3d11_rasterizer_state::get<ID3D11RasterizerState>();
			return S_OK;
		}

		HRESULT __stdcall create_sampler_state(void*, D3D11_SAMPLER_DESC*, ID3D11SamplerState** resource)
		{
			*resource = d3d11_sampler_state::get<ID3D11SamplerState>();
			return S_OK;
		}

		HRESULT __stdcall create_query(void*, D3D11_QUERY_DESC*, ID3D11Query** resource)
		{
			*resource = d3d11_query::get<ID3D11Query>();
			return S_OK;
		}

		HRESULT __stdcall create_predicate(void*, D3D11_QUERY_DESC*, ID3D11Predicate** resource)
		{
			*resource = d3d11_predicate::get<ID3D11Predicate>();
			return S_OK;
		}

		HRESULT __stdcall create_counter(void*, D3D11_COUNTER_DESC*, ID3D11Counter** resource)
		{
			*resource = d3d11_counter::get<ID3D11Counter>();
			return S_OK;
		}

		HRESULT __stdcall create_deferred_context(void* d, UINT a, ID3D11DeviceContext** resource)
		{
			*resource = d3d11_device_context::get<ID3D11DeviceContext>();
			return S_OK;
		}

		HRESULT __stdcall open_shared_resource(void*, HANDLE, IID*, void**)
		{
			return E_NOTIMPL;
		}

		HRESULT __stdcall check_format_support(void*, DXGI_FORMAT, UINT* res)
		{
			*res = 0xFFFFFFFF;
			return S_OK;
		}

		HRESULT __stdcall check_multisample_quality_levels(void*, DXGI_FORMAT, UINT, UINT*)
		{
			return S_OK;
		}

		void __stdcall check_counter_info(void*, D3D11_COUNTER_INFO*)
		{

		}

		HRESULT __stdcall check_counter(void*, D3D11_COUNTER_DESC*, D3D11_COUNTER_TYPE*, UINT*, LPSTR, UINT*, LPSTR, UINT*, LPSTR, UINT*)
		{
			return S_OK;
		}

		HRESULT __stdcall check_feature_support(void*, D3D11_FEATURE, void*, UINT)
		{
			return S_OK;
		}

		HRESULT __stdcall get_private_data(void*, GUID*, UINT* size, void*)
		{
			*size = 0;
			return S_OK;
		}

		HRESULT __stdcall set_private_data(void*, GUID*, UINT, void*)
		{
			return S_OK;
		}

		HRESULT __stdcall set_private_data_interface(void*, GUID*, IUnknown*)
		{
			return S_OK;
		}

		D3D_FEATURE_LEVEL __stdcall get_feature_level(void*)
		{
			return D3D_FEATURE_LEVEL_11_0;
		}

		UINT __stdcall get_creation_flags(void*)
		{
			return 0;
		}

		HRESULT __stdcall get_device_removed_reason(void*)
		{
			return S_OK;
		}

		void __stdcall get_immediate_context(void*, ID3D11DeviceContext** context)
		{
			*context = d3d11_device_context::get<ID3D11DeviceContext>();
		}

		HRESULT __stdcall set_exception_mode(void*, UINT)
		{
			return S_OK;
		}

		UINT __stdcall get_exception_mode(void*)
		{
			return 0;
		}
	}

	d3d11_device* d3d11_device::create()
	{
		static d3d11_device instance{};
		static d3d11_device::vtable vtable{};
		instance.__vftable = &vtable;

		unknown::initialize(&vtable);
		vtable.create_buffer = create_buffer;
		vtable.create_texture_1d = create_texture_1d;
		vtable.create_texture_2d = create_texture_2d;
		vtable.create_texture_3d = create_texture_3d;
		vtable.create_shader_resource_view = create_shader_resource_view;
		vtable.create_unordered_access_view = create_unordered_access_view;
		vtable.create_render_target_view = create_render_target_view;
		vtable.create_depth_stencil_view = create_depth_stencil_view;
		vtable.create_input_layout = create_input_layout;
		vtable.create_vertex_shader = create_vertex_shader;
		vtable.create_geometry_shader = create_geometry_shader;
		vtable.create_geometry_shader_with_stream_output = create_geometry_shader_with_stream_output;
		vtable.create_pixel_shader = create_pixel_shader;
		vtable.create_hull_shader = create_hull_shader;
		vtable.create_domain_shader = create_domain_shader;
		vtable.create_compute_shader = create_compute_shader;
		vtable.create_class_linkage = create_class_linkage;
		vtable.create_blend_state = create_blend_state;
		vtable.create_depth_stencil_state = create_depth_stencil_state;
		vtable.create_rasterizer_state = create_rasterizer_state;
		vtable.create_sampler_state = create_sampler_state;
		vtable.create_query = create_query;
		vtable.create_predicate = create_predicate;
		vtable.create_counter = create_counter;
		vtable.create_deferred_context = create_deferred_context;
		vtable.open_shared_resource = open_shared_resource;
		vtable.check_format_support = check_format_support;
		vtable.check_multisample_quality_levels = check_multisample_quality_levels;
		vtable.check_counter_info = check_counter_info;
		vtable.check_counter = check_counter;
		vtable.check_feature_support = check_feature_support;
		vtable.get_private_data = get_private_data;
		vtable.set_private_data = set_private_data;
		vtable.set_private_data_interface = set_private_data_interface;
		vtable.get_feature_level = get_feature_level;
		vtable.get_creation_flags = get_creation_flags;
		vtable.get_device_removed_reason = get_device_removed_reason;
		vtable.get_immediate_context = get_immediate_context;
		vtable.set_exception_mode = set_exception_mode;
		vtable.get_exception_mode = get_exception_mode;

		return &instance;
	}

	d3d11_device* d3d11_device::get_instance()
	{
		static const auto instance = create();
		return instance;
	}
}
