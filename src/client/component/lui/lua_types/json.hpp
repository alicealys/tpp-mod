#pragma once

#include "../scripting.hpp"

namespace lui::scripting::json
{
	nlohmann::json lua_to_json(const sol::lua_value& value);
	void setup(sol::state& state);
}
