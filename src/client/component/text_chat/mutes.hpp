#pragma once

namespace text_chat::mutes
{
	using mute_list_t = std::unordered_set<std::uint64_t>;

	void mute_player(const std::string& identifier, bool unmute);
	bool is_muted(const std::uint64_t steam_id);
}
