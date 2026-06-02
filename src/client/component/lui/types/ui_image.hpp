#pragma once

#include "ui_element.hpp"

namespace lui
{
	class ui_image;
	using ui_image_ptr = std::shared_ptr<ui_image>;

	enum texture_type_t
	{
		TEXTURE_BASE,
		TEXTURE_SCREEN,
		TEXTURE_MASK,
		TEXTURE_LAYER,
		TEXTURE_COUNT,
	};

	struct texture_t
	{
		game::fox::gr::ResourceId texture{};
		float uv_shift[2]{};
		float uv_repeat[2]{};
		float uv_center[2]{};
		float blend{};
	};

	class ui_image : public ui_element
	{
	public:
		ui_image();
		~ui_image();

		static ui_image_ptr create();

		void set_shader(const std::uint64_t hash);
		void set_shader(const std::string& path);

		void set_texture(const std::uint64_t hash, const std::uint32_t type = TEXTURE_BASE);
		void set_texture(const std::string& path, const std::uint32_t type = TEXTURE_BASE);

		void set_uv_center(float u, float v, const std::uint32_t type = TEXTURE_BASE);
		void set_uv_shift(float u, float v, const std::uint32_t type = TEXTURE_BASE);
		void set_uv_repeat(float u, float v, const std::uint32_t type = TEXTURE_BASE);

		void set_blend(float blend, const std::uint32_t type = TEXTURE_BASE);

		void set_random_uv_shift(bool enabled);

	private:
		void draw_internal(const draw_info_t& inherit) const override;
		void update() override;

		void bind_param(const std::uint32_t param, const float x, const float y, const float z, const float w);
		void update_material();
		void free_texture_resource(const std::uint32_t type);

		game::fox::gr::Material* material_{};
		std::uint64_t shader_{};
		texture_t textures_[TEXTURE_COUNT]{};
		bool randomize_shift_{};
		std::uint32_t last_update_{};

	};
}
