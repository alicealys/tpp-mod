#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game_socket.hpp"
#include "command.hpp"
#include "vars.hpp"
#include "session.hpp"
#include "mutes.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace radio_messages
{
	namespace
	{
		vars::var_ptr radio_ignore;

		bool on_radio_message_callback(game::fox::nt::impl::SessionImpl2* session, const game_socket::message_info_t& info, const std::string_view& buffer)
		{
			if (radio_ignore->current.enabled())
			{
				return true;
			}

			if (buffer.size() < 8)
			{
				return false;
			}

			auto client_num = 0u;
			switch (info.type)
			{
			case 0x49E7EA6B20E0:
				client_num = static_cast<unsigned int>(buffer[1]);
				break;
			case 0xDE553F42B9FE:
				client_num = static_cast<unsigned int>(buffer[3]);
				break;
			case 0x8A2E0942078A:
				client_num = static_cast<unsigned int>(buffer[4]);
				break;
			}

			const auto client = session::get_client_by_index(client_num, nullptr);
			return client != nullptr && mutes::is_muted(client->sessionUserId->userId);
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			if (!game::environment::is_mgo() || game::environment::is_dedi())
			{
				return;
			}

			radio_ignore = vars::register_bool("radio_ignore", false, vars::var_flag_saved, "ignore incoming radio messages");

			game_socket::on(0x49E7EA6B20E0, on_radio_message_callback);
			game_socket::on(0x8A2E0942078A, on_radio_message_callback);
			game_socket::on(0xDE553F42B9FE, on_radio_message_callback);
		}

	};
}

REGISTER_COMPONENT(radio_messages::component)
