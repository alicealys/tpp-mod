#pragma once
#include <d3d11.h>

namespace game
{
	namespace lua
	{
		struct lua_state_container
		{
			void* unk;
			lua_State* state;
			// ...
		};
	}
}
