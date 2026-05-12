#pragma once

#include "interfaces/d3d11/d3d11_pixel_shader.hpp"
#include "interfaces/d3d11/d3d11_vertex_shader.hpp"
#include "interfaces/d3d11/d3d11_device.hpp"
#include "interfaces/d3d11/d3d11_device_context.hpp"
#include "interfaces/d3d11/d3d11_compute_shader.hpp"
#include "interfaces/d3d11/d3d11_texture2d.hpp"
#include "interfaces/d3d11/d3d11_texture3d.hpp"
#include "interfaces/d3d11/d3d11_shader_resource_view.hpp"
#include "interfaces/d3d11/d3d11_buffer.hpp"
#include "interfaces/d3d11/d3d11_blend_state.hpp"
#include "interfaces/d3d11/d3d11_sampler_state.hpp"
#include "interfaces/d3d11/d3d11_input_layout.hpp"
#include "interfaces/d3d11/d3d11_render_target_view.hpp"
#include "interfaces/d3d11/d3d11_depth_stencil_view.hpp"
#include "interfaces/d3d11/d3d11_unordered_access_view.hpp"
#include "interfaces/d3d11/d3d11_rasterizer_state.hpp"
#include "interfaces/d3d11/d3d11_depth_stencil_state.hpp"
#include "interfaces/d3d11/d3d11_query.hpp"

#include "interfaces/dxgi/dxgi_adapter.hpp"
#include "interfaces/dxgi/dxgi_factory.hpp"
#include "interfaces/dxgi/dxgi_output.hpp"

namespace directx
{
	void disable_d3d11();
}
