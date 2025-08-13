#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "custom_server.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>
#include <utils/flags.hpp>

namespace server_logging
{
	namespace
	{
		utils::hook::detour reader_parse_hook;

		bool reader_parse_stub(void* a1, void* a2, const char* begin_doc, const char* end_doc, void* a5)
		{
			const auto dump_command = [&]
			{
				const auto data = std::string{begin_doc, end_doc};
				auto j = nlohmann::json::parse(data);
				if (!j.is_object() || !j["msgid"].is_string())
				{
					return;
				}

				const auto cmd_name = j["msgid"].get<std::string>();
				printf("[server logging] got response for command \"%s\"\n", cmd_name.data());

				static const auto game_name = SELECT_VALUE("tpp", "mgo");
				const auto folder = custom_server::is_using_custom_server() ? "server_dump_custom" : "server_dump";

				const auto name = utils::string::va("tpp-mod/%s/%s/%s/%lli.json", folder, game_name,
					cmd_name.data(), GetTickCount64());

				utils::io::write_file(name, j.dump(4));
			};

			dump_command();
			return reader_parse_hook.invoke<bool>(a1, a2, begin_doc, end_doc, a5);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!utils::flags::has_flag("-server-logging"))
			{
				return;
			}

			reader_parse_hook.create(SELECT_VALUE(0x14D35E4A0, 0x14A509020), reader_parse_stub);
		}
	};
}

REGISTER_COMPONENT(server_logging::component)
