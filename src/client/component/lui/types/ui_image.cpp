#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "ui_image.hpp"
#include "../main.hpp"
#include "../renderer.hpp"
#include "../utils.hpp"

namespace lui
{
	ui_image::ui_image()
	{
		this->id_ = "uiimage";
	}

	void ui_image::draw_internal(const draw_info_t& draw_info) const
	{
		const auto& state = this->animation_state_.current_state;

		float color[4]{};
		color[0] = state.color.r;
		color[1] = state.color.g;
		color[2] = state.color.b;
		color[3] = draw_info.alpha;

		const auto width = (draw_info.rect.right - draw_info.rect.left);
		const auto height = (draw_info.rect.bottom - draw_info.rect.top);

		float uv[4]{};
		if (this->is_uv_set_)
		{
			uv[0] = this->uv_[0];
			uv[1] = this->uv_[1];
			uv[2] = this->uv_[2];
			uv[3] = this->uv_[3];
		}
		else
		{
			uv[0] = 0.f;
			uv[1] = 1.f;
			uv[2] = 1.f;
			uv[3] = 0.f;
		}

		renderer::add_draw_material(this->material_, this->texture_, draw_info.rect.left, draw_info.rect.top, width, height, color, draw_info.rotation, uv);
	}

	void ui_image::set_material(const std::uint64_t hash)
	{
		this->material_ = utils::get_material_resource(hash);
	}

	void ui_image::set_material(const std::string& path)
	{
		this->material_ = utils::get_material_resource(path);
	}

	void ui_image::set_material_resource(const std::uint32_t resource)
	{
		this->material_ = resource;
	}

	void ui_image::set_uv(const float width, const float height, 
		const float x1, const float y1, const float x2, const float y2)
	{
		this->is_uv_set_ = true;
		this->uv_[0] = x1 / width;
		this->uv_[1] = 1.f - y1 / height;
		this->uv_[2] = x2 / width;
		this->uv_[3] = 1.f - y2 / height;
	}

	void ui_image::set_texture(const std::uint64_t hash)
	{
		this->texture_ = utils::get_texture_resource(hash);
	}

	void ui_image::set_texture(const std::string& path)
	{
		this->texture_ = utils::get_texture_resource(path);
	}

	void ui_image::set_texture_resource(const std::uint32_t resource)
	{
		this->texture_ = resource;
	}

	ui_image_ptr ui_image::create()
	{
		return std::make_shared<ui_image>();
	}
}
