#pragma once

namespace custom_server
{
	bool is_using_custom_server();

	std::optional<nlohmann::json> send_command(const std::string& endpoint, const nlohmann::json& data, bool use_session);
}
