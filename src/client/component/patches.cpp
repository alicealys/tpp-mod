#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

namespace patches
{
	namespace
	{

	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			utils::hook::jump(0x14008130A, 0x1400814D8); // unlock framerate always
		}
	};
}

REGISTER_COMPONENT(patches::component)
