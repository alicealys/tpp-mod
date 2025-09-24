#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "custom_server.hpp"
#include "vars.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>
#include <utils/flags.hpp>

namespace server_logging
{
	namespace
	{
		utils::hook::detour http_codec_begin_encode_hook;
		utils::hook::detour http_codec_end_decode_hook;

		vars::var_ptr var_server_logging;

		std::string get_dump_path(const std::string cmd_name, const bool request)
		{
			static const auto game_name = SELECT_VALUE("tpp", "mgo");
			static const auto folder = custom_server::is_using_custom_server() ? "server_dump/custom" : "server_dump/konami";

			const auto request_folder = request ? "requests" : "responses";
			const auto name = utils::string::va("tpp-mod/%s/%s/%s/%s/%lli.json", folder, game_name, request_folder,
				cmd_name.data(), GetTickCount64());

			return name;
		}

		std::string get_fox_buffer(game::fox::Buffer* buffer)
		{
			const auto buf = game::fox::Buffer_::GetBuffer(buffer);
			const auto buf_size = game::fox::Buffer_::GetSize(buffer);
			const auto data = std::string{buf, buf + buf_size};
			return data;
		}

		void* http_codec_end_decode_stub(void* this_, void* ctx, game::fox::Buffer* buffer)
		{
			const auto res = http_codec_end_decode_hook.invoke<void*>(this_, ctx, buffer);

			if (var_server_logging->current.enabled())
			{
				const auto data = get_fox_buffer(buffer);
				const auto json = nlohmann::json::parse(data);
				const auto cmd = json["msgid"].get<std::string>();

				printf("[server logging] received response for command \"%s\"", cmd.data());

				const auto path = get_dump_path(cmd, false);
				utils::io::write_file(path, json.dump(4));
			}

			return res;
		}

		void* http_codec_begin_encode_stub(void* this_, void* ctx, game::fox::Buffer* buffer, void* session_key)
		{
			if (var_server_logging->current.enabled())
			{
				const auto data = get_fox_buffer(buffer);
				const auto json = nlohmann::json::parse(data);
				const auto cmd = json["msgid"].get<std::string>();

				printf("[server logging] sending request for command \"%s\"", cmd.data());

				const auto path = get_dump_path(cmd, true);
				utils::io::write_file(path, json.dump(4));
			}

			return http_codec_begin_encode_hook.invoke<void*>(this_, ctx, buffer, session_key);
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			var_server_logging = vars::register_bool("net_server_logging", false, vars::var_flag_saved, "enable server logging");
		}

		void start() override
		{
			http_codec_begin_encode_hook.create(SELECT_VALUE(0x14D343690, 0x14A4E7640), http_codec_begin_encode_stub);
			http_codec_end_decode_hook.create(SELECT_VALUE(0x141CE3210, 0x140C42A20), http_codec_end_decode_stub);
		}
	};
}

REGISTER_COMPONENT(server_logging::component)
