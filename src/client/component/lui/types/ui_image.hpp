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
		void set_texture(const std::string& path);
		void set_texture(const std::uint64_t hash);
		void set_texture_resource(const std::uint32_t resource);
		void set_uv(const float width, const float height,
			const float x1, const float y1, const float x2, const float y2);

	private:
		void draw_internal(const draw_info_t& inherit) const override;
		game::fox::gr::Material* material_ = nullptr;
		std::uint32_t texture_{};
		float uv_[4]{};
		bool is_uv_set_{};

	};
}
