#pragma once

namespace hashes
{
	const char* lookup(const game::fox::StringId id);
	const char* lookup(const std::uint64_t id);
	game::fox::StringId compute(const char* str);
	game::fox::StringId compute(const std::string& str);
}
