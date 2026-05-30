#pragma once

#pragma warning(push)
#pragma warning(disable: 4459)
#pragma warning(disable: 4702)
#pragma warning(disable: 5321)

#define SOL_ALL_SAFETIES_ON 1
#define SOL_PRINT_ERRORS 0
#include <sol/sol.hpp>

namespace lui::scripting
{
	void start();
	void run_frame();
	void stop();
}
