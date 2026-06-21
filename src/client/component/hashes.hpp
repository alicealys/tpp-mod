#pragma once

namespace hashes
{
	std::string lookup(const game::fox::StringId id);
	std::string lookup(const std::uint64_t id);
	std::string lookup_l(const std::uint32_t id);
	game::fox::StringId compute(const char* str);
	game::fox::StringId compute(const std::string& str);
}
