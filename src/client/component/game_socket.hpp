#pragma once

#include "game/game.hpp"

namespace game_socket
{
	struct message_info_t
	{
		std::uint64_t type;
		std::uint8_t peer_type;
		std::uint16_t game_object_id;
		std::int8_t sender;
		std::int8_t target;
	};

	using handler_t = std::function<void(game::fox::nt::impl::SessionImpl2*, const message_info_t&, const std::string_view&)>;

	void send_raw(const std::uint64_t type, const void* buffer, const std::size_t size, const std::int8_t client, const std::uint8_t peer_type);
	void send(const std::uint64_t message_type, const std::string_view& buffer, const std::int8_t client, const std::uint8_t peer_type = game::fox::gm::PEER_DIRECT);
	void send(const std::string& message_type, const std::string_view& buffer, const std::int8_t client, const std::uint8_t peer_type = game::fox::gm::PEER_DIRECT);
	void on(const std::string& message_type, const handler_t& handler);
}
