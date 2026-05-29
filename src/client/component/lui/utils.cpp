#include <std_include.hpp>

#include "game/game.hpp"

#include "utils.hpp"

namespace lui::utils
{
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
