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
		enum : std::uint32_t
		{
			Base_Texture = 0x3BBF9889,
			Screen_Texture = 0xC97E5580,
			Mask_Texture = 0xBCAE534B,
			Layer_Texture = 0x8D982B8E,

			UCenter_BaseTex = 0xF38B8E7A,
			VCenter_BaseTex = 0x7377E3CF,
			UShift_BaseTex = 0x3E2EF6C2,
			VShift_BaseTex = 0xA1B43647,
			URepeat_BaseTex = 0x96B29F61,
			VRepeat_BaseTex = 0x540F4165,
			Blend_BaseTex = 0xECE504CB,

			UCenter_ScreenTex = 0x882D69FD,
			VCenter_ScreenTex = 0x51FB4EEF,
			UShift_ScreenTex = 0x2FC9FA66,
			VShift_ScreenTex = 0x1B6BF7D5,
			URepeat_ScreenTex = 0xC5A5EE70,
			VRepeat_ScreenTex = 0xA8903ABD,
			Blend_ScreenTex = 0x4E41757F,

			UCenter_MaskTex = 0x8E422B3F,
			VCenter_MaskTex = 0x54C9DCEA,
			UShift_MaskTex = 0xA707068B,
			VShift_MaskTex = 0x987BF357,
			URepeat_MaskTex = 0x8D3C1EBC,
			VRepeat_MaskTex = 0x4CCC0901,
			Blend_MaskTex = 0x2D45F6E6,

			UCenter_LayerTex = 0x5ECDD3BC,
			VCenter_LayerTex = 0x44D0D944,
			UShift_LayerTex = 0xB2BA8021,
			VShift_LayerTex = 0x12168AEE,
			URepeat_LayerTex = 0x934392F8,
			VRepeat_LayerTex = 0x75DC38D5,
			Blend_LayerTex = 0x2F965F29,
		};


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
			texture_type_names[TEXTURE_BASE] = Base_Texture;
			texture_type_names[TEXTURE_SCREEN] = Screen_Texture;
			texture_type_names[TEXTURE_MASK] = Mask_Texture;
			texture_type_names[TEXTURE_LAYER] = Layer_Texture;

			material_param_names[TEXTURE_BASE].ucenter = UCenter_BaseTex;
			material_param_names[TEXTURE_BASE].vcenter = VCenter_BaseTex;
			material_param_names[TEXTURE_BASE].ushift = UShift_BaseTex;
			material_param_names[TEXTURE_BASE].vshift = VShift_BaseTex;
			material_param_names[TEXTURE_BASE].urepeat = URepeat_BaseTex;
			material_param_names[TEXTURE_BASE].vrepeat = VRepeat_BaseTex;
			material_param_names[TEXTURE_BASE].blend = Blend_BaseTex;

			material_param_names[TEXTURE_SCREEN].ucenter = UCenter_ScreenTex;
			material_param_names[TEXTURE_SCREEN].vcenter = VCenter_ScreenTex;
			material_param_names[TEXTURE_SCREEN].ushift = UShift_ScreenTex;
			material_param_names[TEXTURE_SCREEN].vshift = VShift_ScreenTex;
			material_param_names[TEXTURE_SCREEN].urepeat = URepeat_ScreenTex;
			material_param_names[TEXTURE_SCREEN].vrepeat = VRepeat_ScreenTex;
			material_param_names[TEXTURE_SCREEN].blend = Blend_ScreenTex;

			material_param_names[TEXTURE_MASK].ucenter = UCenter_MaskTex;
			material_param_names[TEXTURE_MASK].vcenter = VCenter_MaskTex;
			material_param_names[TEXTURE_MASK].ushift = UShift_MaskTex;
			material_param_names[TEXTURE_MASK].vshift = VShift_MaskTex;
			material_param_names[TEXTURE_MASK].urepeat = URepeat_MaskTex;
			material_param_names[TEXTURE_MASK].vrepeat = VRepeat_MaskTex;
			material_param_names[TEXTURE_MASK].blend = Blend_MaskTex;

			material_param_names[TEXTURE_LAYER].ucenter = UCenter_LayerTex;
			material_param_names[TEXTURE_LAYER].vcenter = VCenter_LayerTex;
			material_param_names[TEXTURE_LAYER].ushift = UShift_LayerTex;
			material_param_names[TEXTURE_LAYER].vshift = VShift_LayerTex;
			material_param_names[TEXTURE_LAYER].urepeat = URepeat_LayerTex;
			material_param_names[TEXTURE_LAYER].vrepeat = VRepeat_LayerTex;
			material_param_names[TEXTURE_LAYER].blend = Blend_LayerTex;
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

		game::fox::StringId material_id{};
		material_id.id = 0x189F16095D8A;
		game::fox::gr::Material_::Create(&this->material_, &material_id);

		for (auto i = 0; i < TEXTURE_COUNT; i++)
		{
			this->textures_[i].texture = 0;

			this->textures_[i].uv_center[0] = 0.f;
			this->textures_[i].uv_center[1] = 0.f;

			this->textures_[i].uv_shift[0] = 0.f;
			this->textures_[i].uv_shift[1] = 0.f;

			this->textures_[i].uv_repeat[0] = 1.f;
			this->textures_[i].uv_repeat[1] = 1.f;

			this->textures_[i].blend = 1.f;
		}

		this->set_shader("fox_2d_Basic_LyBL");
		this->set_texture("/Assets/tpp/common_source/ui/common_texture/cm_wht_64.ftex");
	}

	ui_image::~ui_image()
	{
		if (this->material_ != nullptr)
		{
			this->material_->__vftable->__destructor(this->material_, 1);
			this->material_ = nullptr;
		}
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

		renderer::add_draw_material(this->material_->resource, draw_info.rect.left, draw_info.rect.top, width, height, color, draw_info.rotation);
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
		this->textures_[type].texture = utils::get_texture_resource(hash);
		this->update_material();
	}

	void ui_image::set_texture(const std::string& path, const std::uint32_t type)
	{
		this->textures_[type].texture = utils::get_texture_resource(path);
		this->update_material();
	}

	void ui_image::set_blend(const float blend, const std::uint32_t type)
	{
		this->textures_[type].blend = blend;
		this->update_material();
	}

	void ui_image::update_material()
	{
		game::fox::StringId technique{};
		technique.id = this->shader_;
		game::fox::gr::Material_::BindShaderTechnique(this->material_, &technique);

		const auto bind_param = [&](unsigned int name, const float x, const float y, const float z, const float w)
		{
			game::Vectormath::Aos::Vector4 vec{};
			vec.values[0] = x;
			vec.values[1] = y;
			vec.values[2] = z;
			vec.values[3] = w;
			game::fox::gr::Material_::BindParameter(this->material_, name, &vec);
		};

		for (auto i = 0; i < TEXTURE_COUNT; i++)
		{
			if (this->textures_[i].texture == 0)
			{
				continue;
			}

			game::fox::gr::Texture texture{};
			texture.id = this->textures_[i].texture;

			game::fox::gr::Material_::BindTexture(this->material_, texture_type_names[i], &texture);

			bind_param(material_param_names[i].ucenter, this->textures_[i].uv_center[0], 0.f, 0.f, 0.f);
			bind_param(material_param_names[i].vcenter, this->textures_[i].uv_center[1], 0.f, 0.f, 0.f);
			bind_param(material_param_names[i].ushift, this->textures_[i].uv_shift[0], 0.f, 0.f, 0.f);
			bind_param(material_param_names[i].vshift, this->textures_[i].uv_shift[1], 0.f, 0.f, 0.f);
			bind_param(material_param_names[i].urepeat, this->textures_[i].uv_repeat[0], 0.f, 0.f, 0.f);
			bind_param(material_param_names[i].vrepeat, this->textures_[i].uv_repeat[1], 0.f, 0.f, 0.f);
			bind_param(material_param_names[i].blend, this->textures_[i].blend, 0.f, 0.f, 0.f);
		}
	}

	ui_image_ptr ui_image::create()
	{
		const auto image = std::make_shared<ui_image>();
		image->track();
		return image;
	}
}
