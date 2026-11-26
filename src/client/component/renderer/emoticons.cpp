#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "utils.hpp"
#include "emoticons.hpp"
#include "draw.hpp"
#include "../console.hpp"
#include <utils/string.hpp>
#include <utils/io.hpp>
#include <utils/nt.hpp>

namespace renderer
{
	namespace
	{
		constexpr const auto hash_init = 0x319712C3u;

		std::unordered_map<std::uint32_t, std::shared_ptr<emoticon_t>> emoticons_map;
		std::vector<std::shared_ptr<emoticon_t>> emoticons_list;

		inline void hash_char(std::uint32_t& hash, char c)
		{
			hash = 0xB3CB2E29 * static_cast<std::uint32_t>(std::tolower(c) ^ hash);
		}

		std::uint32_t hash_string(const char* c)
		{
			std::uint32_t hash = hash_init;
			while (*c)
			{
				hash_char(hash, *c);
				++c;
			}
			return hash;
		}

		void bind_material_param(game::fox::gr::Material* material, const std::uint32_t name, const float x, const float y, const float z, const float w)
		{
			game::Vectormath::Aos::Vector4 vec{};
			vec.values[0] = x;
			vec.values[1] = y;
			vec.values[2] = z;
			vec.values[3] = w;
			game::fox::gr::Material_::BindParameter(material, name, &vec);
		}

		bool initialize_emoticon(emoticon_t* emoticon, const std::uint64_t texture_id, float* uv_repeat, float* uv_shift)
		{
			game::fox::Path path{};
			path.id = texture_id;

			game::fox::gr::Texture texture{};
			texture.id = game::fox::gr::dg::TextureManager_::CreateResourceFromFile(&path);

			const auto streamer = game::fox::gr::dg::DgTextureStreamer_::Instance();
			if (game::fox::gr::dg::DgTextureStreamer_::IsEnable(streamer))
			{
				game::fox::gr::dg::DgTextureStreamer_::RequestTextureDetailByDgTexture(streamer, texture.id, 2);
			}

			const auto width = game::fox::gr::Texture_::GetWidth(&texture);
			const auto height = game::fox::gr::Texture_::GetHeight(&texture);

			if (width == 0.f)
			{
				return false;
			}

			emoticon->ratio = width / height;

			game::fox::StringId material_id{};
			material_id.id = 0x189F16095D8A;
			game::fox::gr::Material_::Create(&emoticon->material, &material_id);

			game::fox::StringId technique{};
			technique.id = game::fox_2d_Basic_LyMUL;

			game::fox::gr::Material_::BindShaderTechnique(emoticon->material, &technique);
			game::fox::gr::Material_::BindTexture(emoticon->material, game::Base_Texture, &texture);

			bind_material_param(emoticon->material, game::UCenter_BaseTex, 0.f, 0.f, 0.f, 0.f);
			bind_material_param(emoticon->material, game::VCenter_BaseTex, 0.f, 0.f, 0.f, 0.f);
			bind_material_param(emoticon->material, game::UShift_BaseTex, uv_shift[0], 0.f, 0.f, 0.f);
			bind_material_param(emoticon->material, game::VShift_BaseTex, uv_shift[1], 0.f, 0.f, 0.f);
			bind_material_param(emoticon->material, game::URepeat_BaseTex, uv_repeat[0], 0.f, 0.f, 0.f);
			bind_material_param(emoticon->material, game::VRepeat_BaseTex, uv_repeat[1], 0.f, 0.f, 0.f);
			bind_material_param(emoticon->material, game::Blend_BaseTex, 1.f, 0.f, 0.f, 0.f);

			if ((emoticon->flags & emoticon_flag_mask) != 0)
			{
				game::fox::gr::Material_::BindTexture(emoticon->material, game::Mask_Texture, &texture);
				bind_material_param(emoticon->material, game::UCenter_MaskTex, 0.f, 0.f, 0.f, 0.f);
				bind_material_param(emoticon->material, game::VCenter_MaskTex, 0.f, 0.f, 0.f, 0.f);
				bind_material_param(emoticon->material, game::UShift_MaskTex, uv_shift[0], 0.f, 0.f, 0.f);
				bind_material_param(emoticon->material, game::VShift_MaskTex, uv_shift[1], 0.f, 0.f, 0.f);
				bind_material_param(emoticon->material, game::URepeat_MaskTex, uv_repeat[0], 0.f, 0.f, 0.f);
				bind_material_param(emoticon->material, game::VRepeat_MaskTex, uv_repeat[1], 0.f, 0.f, 0.f);
				bind_material_param(emoticon->material, game::Blend_MaskTex, 1.f, 0.f, 0.f, 0.f);
			}

			return true;
		}
	}

	bool get_emoticon(const char* text, unsigned int& len, emoticon_t** emoticon)
	{
		std::uint32_t hash = hash_init;

		while (text[len] != 0 && len < max_emoticon_length)
		{
			if (text[len] == ':')
			{
				const auto iter = emoticons_map.find(hash);
				if (iter != emoticons_map.end())
				{
					*emoticon = iter->second.get();
					return true;
				}

				return false;
			}
			else
			{
				hash_char(hash, static_cast<char>(text[len]));
			}

			++len;
		}

		return false;
	}

	bool get_emoticon(const wchar_t* text, unsigned int& len, emoticon_t** emoticon)
	{
		std::uint32_t hash = hash_init;

		while (text[len] != 0 && len < max_emoticon_length)
		{
			if (text[len] == ':')
			{
				++len;
				const auto iter = emoticons_map.find(hash);
				if (iter != emoticons_map.end())
				{
					*emoticon = iter->second.get();
					return true;
				}

				return false;
			}
			else
			{
				hash_char(hash, static_cast<char>(std::wctob(text[len])));
			}

			++len;
		}

		return false;
	}

	float draw_emoticon(game::fox::gr::dg::plugins::Draw2DRenderer* instance, emoticon_t* emoticon, float x, float y, 
		float size, float alpha, bool add_color, float* color_override)
	{
		float color[4]{};
		color[0] = emoticon->color[0];
		color[1] = emoticon->color[1];
		color[2] = emoticon->color[2];
		color[3] = emoticon->color[3] * alpha;

		const auto width = size * emoticon->ratio;

		execute_push(instance, 0xFF);

		if (color_override != nullptr)
		{
			set_color(instance, color_override);
		}

		if (add_color)
		{
			set_color(instance, color);
		}

		set_material(instance, emoticon->material);
		add_box2(instance, x, y - size, width, size);
		set_material(instance, nullptr);
		execute_pop(instance, 0xFF);

		return width;
	}

	void find_emoticons(const std::string& input, std::vector<std::shared_ptr<emoticon_t>>& matches)
	{
		for (const auto& emo : emoticons_list)
		{
			if (emo->name.contains(input))
			{
				matches.emplace_back(emo);
			}
		}
	}

	void load_emoticons()
	{
		const auto emoticon_list_data = utils::nt::load_resource(RESOURCE_EMOTICON_LIST);
		auto data = nlohmann::json::parse(emoticon_list_data, nullptr, false);
		if (data.is_discarded())
		{
			console::error("[renderer::emoticons] failed to load emoticon list\n");
			return;
		}

		for (auto i = 0ull; i < data.size(); i++)
		{
			auto& entry_j = data[i];
			if (!entry_j.is_object())
			{
				continue;
			}

			auto& name_j = entry_j["name"];
			auto& texture_j = entry_j["texture"];
			auto& game_j = entry_j["game"];
			auto& flags_j = entry_j["flags"];
			auto& color_j = entry_j["color"];
			auto& uv_repeat_j = entry_j["uv_repeat"];
			auto& uv_shift_j = entry_j["uv_shift"];
			auto& ratio_j = entry_j["ratio"];

			if (!name_j.is_string() || !texture_j.is_string() || !game_j.is_number_unsigned() || 
				!flags_j.is_number_unsigned() || !color_j.is_array() || 
				!uv_repeat_j.is_array() || !uv_shift_j.is_array())
			{
				continue;
			}

			auto game = game_j.get<std::uint32_t>();
			const auto target_game = game::environment::is_tpp() ? emoticon_game_tpp : emoticon_game_mgo;
			if (game != 0 && game != target_game)
			{
				continue;
			}

			auto emoticon = std::make_shared<emoticon_t>();
			emoticon->name = name_j.get<std::string>();
			emoticon->flags = flags_j.get<std::uint32_t>();
			emoticon->color[0] = color_j[0].get<float>();
			emoticon->color[1] = color_j[1].get<float>();
			emoticon->color[2] = color_j[2].get<float>();

			if (color_j.size() == 4)
			{
				emoticon->color[3] = color_j[3].get<float>();
			}
			else
			{
				emoticon->color[3] = 1.f;
			}

			float uv_repeat[2]{};
			float uv_shift[2]{};

			uv_repeat[0] = uv_repeat_j[0].get<float>();
			uv_repeat[1] = uv_repeat_j[1].get<float>();

			uv_shift[0] = uv_shift_j[0].get<float>();
			uv_shift[1] = uv_shift_j[1].get<float>();

			const auto texture_name = texture_j.get<std::string>();
			const auto texture_id = std::strtoull(texture_name.data(), nullptr, 16);

			if (!initialize_emoticon(emoticon.get(), texture_id, uv_repeat, uv_shift))
			{
				console::error("[renderer::emoticons] emoticon %s has no texture\n", emoticon->name.data());
				continue;
			}

			if (ratio_j.is_number())
			{
				emoticon->ratio = ratio_j.get<float>();
			}

			const auto hash = hash_string(emoticon->name.data());
			if (emoticons_map.contains(hash))
			{
				console::error("[renderer::emoticons] hash collision on %s\n", emoticon->name.data());
				continue;
			}

			emoticons_map.insert(std::make_pair(hash, emoticon));
			emoticons_list.emplace_back(emoticon);
		}
	}

	void destroy_emoticons()
	{
		for (auto& emoticon : emoticons_list)
		{
			if (emoticon->material != nullptr)
			{
				emoticon->material->__vftable->__destructor(emoticon->material, 1);
				emoticon->material = nullptr;
			}
		}

		emoticons_map.clear();
		emoticons_list.clear();
	}

	namespace emoticons
	{
		class component final : public component_interface
		{
		public:
			void pre_load() override
			{

			}

			void game_initialized() override
			{
				load_emoticons();
			}

			void end() override
			{
				destroy_emoticons();
			}
		};
	}
}

REGISTER_COMPONENT(renderer::emoticons::component)
