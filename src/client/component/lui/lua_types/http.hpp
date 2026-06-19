#pragma once

#include "../scripting.hpp"

namespace lui::scripting::http
{
	void setup(sol::state& state);
	void run_frame(sol::state& state);
	void clear();
}
