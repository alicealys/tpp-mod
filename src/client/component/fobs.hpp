#pragma once

#include "game/game.hpp"

namespace fobs
{
	using custom_fob_targets_t = std::unordered_map<std::string, std::vector<game::tpp::mbm::PlayerBasicInfo>>;

	void add_custom_fob_target(const std::string& type, const game::tpp::mbm::PlayerBasicInfo& info);
	void remove_custom_fob_target(const std::string& type, const std::uint64_t steam_id);
	void clear_custom_fob_targets();
	void access_custom_fob_targets(const std::function<void(custom_fob_targets_t&)> callback);
	std::uint32_t get_own_player_id();
}
