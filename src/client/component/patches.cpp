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
			utils::hook::jump(SELECT_VALUE(0x14008130A, 0x14008195A), SELECT_VALUE(0x1400814D8, 0x140081B28)); // unlock framerate always
		}
	};
}

REGISTER_COMPONENT(patches::component)
