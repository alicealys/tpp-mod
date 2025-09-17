#include <std_include.hpp>
#include "game.hpp"

#include <utils/flags.hpp>

namespace game
{
	namespace environment
	{
		game_mode mode = game_mode::none;

		void set_mode(const game_mode mode_)
		{
			mode = mode_;
		}

		game_mode get_mode()
		{
			return mode;
		}

		bool is_mgsv()
		{
			return *reinterpret_cast<int*>(0x141001337) == 0x24448B48;
		}

		bool is_mgo()
		{
			return !is_mgsv();
		}

		bool is_dedi()
		{
			return is_mgo() && utils::flags::has_flag("dedicated");
		}
	}
}
