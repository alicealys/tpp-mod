#pragma once

#include "command.hpp"

namespace binds
{
	void add_action_command(const std::string& name, const command::callback& cb);
}
