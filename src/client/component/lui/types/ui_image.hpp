#pragma once

#include "ui_element.hpp"

namespace lui
{
	class ui_image;
	using ui_image_ptr = std::shared_ptr<ui_image>;

	class ui_image : public ui_element
	{
	public:
		ui_image();

		static ui_image_ptr create();

		void set_material(game::fox::gr::Material* material);

	private:
		void draw_internal(const draw_info_t& inherit) const override;
		game::fox::gr::Material* material_ = nullptr;

	};
}
