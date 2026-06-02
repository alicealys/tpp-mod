#include <std_include.hpp>

#include "utils.hpp"

namespace lui::utils
{
	game::fox::gr::ResourceId get_material_resource(const std::uint64_t hash)
	{
		if (hash == 0)
		{
			return {};
		}

		game::fox::StringId id{};
		id.id = hash;

		game::fox::gr::Material material{};
		game::fox::gr::Material_::Material_(&material, &id);
		return material.resource;
	}

	game::fox::gr::ResourceId get_material_resource(const std::string& path)
	{
		const auto hash = game::fox::fs::PathCodeImpl_::FromString(path.data());
		return get_material_resource(hash);
	}

	game::fox::gr::ResourceId get_texture_resource(const std::uint64_t hash)
	{
		if (hash == 0)
		{
			return {};
		}

		game::fox::Path path{};
		path.id = hash;
		return game::fox::gr::dg::TextureManager_::CreateResourceFromFile(&path);
	}

	game::fox::gr::ResourceId get_texture_resource(const std::string& path)
	{
		const auto hash = game::fox::fs::PathCodeImpl_::FromString(path.data());
		return get_texture_resource(hash);
	}

	void play_sound(const std::uint32_t id)
	{
		const auto ui_inst = game::tpp::ui::menu::UiCommonDataManager_::GetInstance();
		if (ui_inst == nullptr)
		{
			return;
		}

		const auto sound_control = game::tpp::ui::menu::UiCommonDataManager_::GetSoundControl(ui_inst);
		if (sound_control == nullptr)
		{
			return;
		}

		game::tpp::ui::utility::StartSound(sound_control, id);
	}
}
