#pragma once

namespace session
{
	struct session_info_t
	{
		bool active;
		bool is_host;
		bool is_connected;
		std::int32_t state;
	};

	extern session_info_t session_info;

	bool is_host();
	game::fox::nt::Member* get_client_by_index(const unsigned int index, bool* is_self);
	game::fox::nt::Member* get_client_by_steam_id(const std::uint64_t steam_id, bool* is_self = nullptr, int* index = nullptr);
	game::fox::nt::Member* get_client_by_name(const std::string& name, bool* is_self, std::string* out_name = nullptr);
	game::fox::nt::Member* find_client(const std::string& identifier, bool* is_self = nullptr);

	int get_rtt(game::fox::nt::impl::SessionImpl2* session);
	bool is_host(game::fox::nt::impl::SessionImpl2* session);
	unsigned int get_state(game::fox::nt::impl::SessionImpl2* session);
	void connect_to_lobby(const game::steam_id lobby_id);

	game::tpp::mp::Ruleset* get_active_ruleset();
	char get_player_team(const std::uint64_t steam_id);
	char get_self_team();
	const char* get_player_name(const unsigned char index);
}
