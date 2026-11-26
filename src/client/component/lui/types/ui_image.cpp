#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "ui_image.hpp"
#include "../main.hpp"
#include "../renderer.hpp"
#include "../utils.hpp"

namespace lui
{
	namespace
	{
		struct material_param_names_t
		{
			std::uint32_t ucenter;
			std::uint32_t vcenter;
			std::uint32_t ushift;
			std::uint32_t vshift;
			std::uint32_t urepeat;
			std::uint32_t vrepeat;
			std::uint32_t blend;
		};

		std::uint32_t texture_type_names[TEXTURE_COUNT]{};
		material_param_names_t material_param_names[TEXTURE_COUNT]{};

		void initialize_names()
		{
			texture_type_names[TEXTURE_BASE] = game::Base_Texture;
			texture_type_names[TEXTURE_SCREEN] = game::Screen_Texture;
			texture_type_names[TEXTURE_MASK] = game::Mask_Texture;
			texture_type_names[TEXTURE_LAYER] = game::Layer_Texture;

			material_param_names[TEXTURE_BASE].ucenter = game::UCenter_BaseTex;
			material_param_names[TEXTURE_BASE].vcenter = game::VCenter_BaseTex;
			material_param_names[TEXTURE_BASE].ushift = game::UShift_BaseTex;
			material_param_names[TEXTURE_BASE].vshift = game::VShift_BaseTex;
			material_param_names[TEXTURE_BASE].urepeat = game::URepeat_BaseTex;
			material_param_names[TEXTURE_BASE].vrepeat = game::VRepeat_BaseTex;
			material_param_names[TEXTURE_BASE].blend = game::Blend_BaseTex;

			material_param_names[TEXTURE_SCREEN].ucenter = game::UCenter_ScreenTex;
			material_param_names[TEXTURE_SCREEN].vcenter = game::VCenter_ScreenTex;
			material_param_names[TEXTURE_SCREEN].ushift = game::UShift_ScreenTex;
			material_param_names[TEXTURE_SCREEN].vshift = game::VShift_ScreenTex;
			material_param_names[TEXTURE_SCREEN].urepeat = game::URepeat_ScreenTex;
			material_param_names[TEXTURE_SCREEN].vrepeat = game::VRepeat_ScreenTex;
			material_param_names[TEXTURE_SCREEN].blend = game::Blend_ScreenTex;

			material_param_names[TEXTURE_MASK].ucenter = game::UCenter_MaskTex;
			material_param_names[TEXTURE_MASK].vcenter = game::VCenter_MaskTex;
			material_param_names[TEXTURE_MASK].ushift = game::UShift_MaskTex;
			material_param_names[TEXTURE_MASK].vshift = game::VShift_MaskTex;
			material_param_names[TEXTURE_MASK].urepeat = game::URepeat_MaskTex;
			material_param_names[TEXTURE_MASK].vrepeat = game::VRepeat_MaskTex;
			material_param_names[TEXTURE_MASK].blend = game::Blend_MaskTex;

			material_param_names[TEXTURE_LAYER].ucenter = game::UCenter_LayerTex;
			material_param_names[TEXTURE_LAYER].vcenter = game::VCenter_LayerTex;
			material_param_names[TEXTURE_LAYER].ushift = game::UShift_LayerTex;
			material_param_names[TEXTURE_LAYER].vshift = game::VShift_LayerTex;
			material_param_names[TEXTURE_LAYER].urepeat = game::URepeat_LayerTex;
			material_param_names[TEXTURE_LAYER].vrepeat = game::VRepeat_LayerTex;
			material_param_names[TEXTURE_LAYER].blend = game::Blend_LayerTex;
		}

		class initializer
		{
		public:
			initializer()
			{
				initialize_names();
			}
		};

		initializer __;
	}

	ui_image::ui_image()
	{
		this->id_ = "uiimage";
		this->type_ = UI_IMAGE;
	}

	ui_image::~ui_image()
	{
		if (this->material_ != nullptr)
		{
			this->material_->__vftable->__destructor(this->material_, 1);
			this->material_ = nullptr;
		}

		this->free_texture_resource(TEXTURE_BASE);
		this->free_texture_resource(TEXTURE_SCREEN);
		this->free_texture_resource(TEXTURE_MASK);
		this->free_texture_resource(TEXTURE_LAYER);
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

		::renderer::params_t renderer_params{};
		renderer_params.rotation = draw_info.rotation;
		std::memcpy(&renderer_params.perspective, &draw_info.perspective, sizeof(renderer_params.perspective));

		renderer::add_draw_material(this->material_->resource.data, draw_info.rect.left, draw_info.rect.top, width, height, color, &renderer_params);
	}

	void ui_image::set_uv_center(float u, float v, const std::uint32_t type)
	{
		this->textures_[type].uv_center[0] = u;
		this->textures_[type].uv_center[1] = v;
		this->update_material();
	}

	void ui_image::set_uv_shift(float u, float v, const std::uint32_t type)
	{
		this->textures_[type].uv_shift[0] = u;
		this->textures_[type].uv_shift[1] = v;
		this->update_material();
	}

	void ui_image::set_uv_repeat(float u, float v, const std::uint32_t type)
	{
		this->textures_[type].uv_repeat[0] = u;
		this->textures_[type].uv_repeat[1] = v;
		this->update_material();
	}

	void ui_image::set_shader(const std::uint64_t hash)
	{
		this->shader_ = hash;
		this->update_material();
	}

	void ui_image::set_shader(const std::string& name)
	{
		const auto hash = game::fox::FoxStrHash32(name.data(), name.size());
		this->set_shader(hash.id);
	}

	void ui_image::set_texture(const std::uint64_t hash, const std::uint32_t type)
	{
		if (hash == 0)
		{
			return;
		}

		game::fox::Path path{};
		path.id = hash;

		const auto resource = game::fox::gr::dg::TextureManager_::CreateResourceFromFile(&path);
		const auto streamer = game::fox::gr::dg::DgTextureStreamer_::Instance();

		if (game::fox::gr::dg::DgTextureStreamer_::IsEnable(streamer))
		{
			game::fox::gr::dg::DgTextureStreamer_::RequestTextureDetailByDgTexture(streamer, resource, 2);
		}

		this->free_texture_resource(type);
		this->textures_[type].texture = resource;
		this->update_material();
	}

	void ui_image::set_texture(const std::string& path, const std::uint32_t type)
	{
		const auto hash = game::fox::fs::PathCodeImpl_::FromString(path.data());
		this->set_texture(hash, type);
	}

	void ui_image::set_blend(const float blend, const std::uint32_t type)
	{
		this->textures_[type].blend = blend;
		this->update_material();
	}

	void ui_image::bind_param(const std::uint32_t name, const float x, const float y, const float z, const float w)
	{
		game::Vectormath::Aos::Vector4 vec{};
		vec.values[0] = x;
		vec.values[1] = y;
		vec.values[2] = z;
		vec.values[3] = w;
		game::fox::gr::Material_::BindParameter(this->material_, name, &vec);
	}

	void ui_image::update_material()
	{
		game::fox::StringId technique{};
		technique.id = this->shader_;
		game::fox::gr::Material_::BindShaderTechnique(this->material_, &technique);

		for (auto i = 0; i < TEXTURE_COUNT; i++)
		{
			if (this->textures_[i].texture.data == 0)
			{
				continue;
			}

			game::fox::gr::Texture texture{};
			texture.id = this->textures_[i].texture;

			game::fox::gr::Material_::BindTexture(this->material_, texture_type_names[i], &texture);

			this->bind_param(material_param_names[i].ucenter, this->textures_[i].uv_center[0], 0.f, 0.f, 0.f);
			this->bind_param(material_param_names[i].vcenter, this->textures_[i].uv_center[1], 0.f, 0.f, 0.f);
			this->bind_param(material_param_names[i].ushift, this->textures_[i].uv_shift[0], 0.f, 0.f, 0.f);
			this->bind_param(material_param_names[i].vshift, this->textures_[i].uv_shift[1], 0.f, 0.f, 0.f);
			this->bind_param(material_param_names[i].urepeat, this->textures_[i].uv_repeat[0], 0.f, 0.f, 0.f);
			this->bind_param(material_param_names[i].vrepeat, this->textures_[i].uv_repeat[1], 0.f, 0.f, 0.f);
			this->bind_param(material_param_names[i].blend, this->textures_[i].blend, 0.f, 0.f, 0.f);
		}
	}

	void ui_image::free_texture_resource(const std::uint32_t type)
	{
		if (this->textures_[type].texture.data != 0)
		{
			const auto streamer = game::fox::gr::dg::DgTextureStreamer_::Instance();
			if (game::fox::gr::dg::DgTextureStreamer_::IsEnable(streamer))
			{
				game::fox::gr::dg::DgTextureStreamer_::UnRequestTextureDetailByDgTexture(streamer, this->textures_[type].texture, 2);
			}

			// game::fox::gr::dg::ResourceManagerBase_::TextureResource_::DeleteResource(this->textures_[type].texture);
			this->textures_[type].texture.data = 0;
		}
	}

	void ui_image::set_random_uv_shift(bool enabled)
	{
		this->randomize_shift_ = enabled;
	}

	void ui_image::update()
	{
		ui_element::update();

		const auto now = get_current_msec();
		if (this->randomize_shift_ && (now - this->last_update_) > 20)
		{
			const auto x = static_cast<float>(rand() % 100) / 100.f;
			const auto y = static_cast<float>(rand() % 100) / 100.f;
			this->set_uv_shift(x, y, TEXTURE_BASE);
			this->bind_param(game::UShift_BaseTex, x, 0.f, 0.f, 0.f);
			this->bind_param(game::VShift_BaseTex, y, 0.f, 0.f, 0.f);
			this->last_update_ = now;
		}
	}

	ui_image_ptr ui_image::create()
	{
		const auto image = std::make_shared<ui_image>();
		image->track();

		game::fox::StringId material_id{};
		material_id.id = 0x189F16095D8A;
		game::fox::gr::Material_::Create(&image->material_, &material_id);

		for (auto i = 0; i < TEXTURE_COUNT; i++)
		{
			image->textures_[i].texture.data = 0;

			image->textures_[i].uv_center[0] = 0.f;
			image->textures_[i].uv_center[1] = 0.f;

			image->textures_[i].uv_shift[0] = 0.f;
			image->textures_[i].uv_shift[1] = 0.f;

			image->textures_[i].uv_repeat[0] = 1.f;
			image->textures_[i].uv_repeat[1] = 1.f;

			image->textures_[i].blend = 1.f;
		}

		image->set_shader("fox_2d_Basic_LyMUL");
		image->set_texture("/Assets/tpp/common_source/ui/common_texture/cm_wht_64.ftex", TEXTURE_BASE);

		return image;
	}
}
