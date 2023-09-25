#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "scheduler.hpp"
#include "console.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>
#include <utils/flags.hpp>

namespace custom_server
{
	namespace
	{
		char custom_url[0x2000]{};
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			static const auto custom_server = utils::flags::get_flag("custom-server");
			if (custom_server.has_value() && custom_server->size() < sizeof(custom_url))
			{
				std::memcpy(custom_url, custom_server->data(), custom_server->size());
				console::info("Using custom server url: %s\n", custom_url);
				utils::hook::inject(0x1407D27AC + 3, custom_url);
			}
		}
	};
}

REGISTER_COMPONENT(custom_server::component)
