#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "scheduler.hpp"
#include "console.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>

#include <discord_rpc.h>

namespace discord
{
	namespace
	{
		DiscordRichPresence discord_presence{};

		const char* get_state()
		{
			return "";
		}

		const char* get_details()
		{
			return SELECT_VALUE("", "Playing Metal Gear Online");
		}
		
		const char* get_icon()
		{
			return SELECT_VALUE("mgsv", "mgo");
		}

		void update_discord()
		{
			Discord_RunCallbacks();

			discord_presence.state = get_state();
			discord_presence.details = get_details();
			discord_presence.largeImageKey = get_icon();

			//if (!discord_presence.startTimestamp)
			//{
			//	discord_presence.startTimestamp = std::chrono::duration_cast<std::chrono::seconds>(
			//		std::chrono::system_clock::now().time_since_epoch()).count();
			//}

			Discord_UpdatePresence(&discord_presence);
		}

		void ready(const DiscordUser* /*request*/)
		{
			std::memset(&discord_presence, 0, sizeof(discord_presence));

			discord_presence.instance = 1;
			discord_presence.state = "";

			Discord_UpdatePresence(&discord_presence);
		}

		void errored(const int error_code, const char* message)
		{
			console::error("Discord: (%i) %s", error_code, message);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			DiscordEventHandlers handlers{};
			
			handlers.ready = ready;
			handlers.errored = errored;
			handlers.disconnected = errored;
			handlers.joinGame = nullptr;
			handlers.spectateGame = nullptr;
			handlers.joinRequest = nullptr;
			
			Discord_Initialize("1104156817845665792", &handlers, 1, nullptr);

			scheduler::loop(update_discord, scheduler::pipeline::async, 5s);
		}
	};
}

REGISTER_COMPONENT(discord::component)
