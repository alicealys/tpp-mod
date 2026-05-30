#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "ui_image.hpp"
#include "../main.hpp"
#include "../renderer.hpp"

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

		renderer::add_draw_material(this->material_, draw_info.rect.left, draw_info.rect.top, width, height, color, draw_info.rotation);
	}

	void ui_image::set_material(game::fox::gr::Material* material)
	{
		this->material_ = material;
	}

	ui_image_ptr ui_image::create()
	{
		return std::make_shared<ui_image>();
	}
}
