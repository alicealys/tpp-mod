#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "hashes.hpp"
#include "command.hpp"
#include "console.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/io.hpp>

namespace hashes
{
	namespace
	{
		utils::memory::allocator allocator;

		std::mutex map_mutex;
		std::unordered_map<std::uint64_t, const char*> string_map;

		utils::hook::detour fox_str_hash_hook;
		utils::hook::detour shared_string_to_string_id_hook;

		game::fox::StringId fox_str_hash_stub(const char* str, size_t size)
		{
			std::lock_guard _0(map_mutex);
			const auto result = fox_str_hash_hook.invoke<game::fox::StringId>(str, size);
			const auto copy = allocator.duplicate_string(std::string{str, str + size});
			string_map.insert(std::make_pair(result.id, copy));
			return result;
		}

		game::fox::StringId* shared_string_to_string_id_stub(game::fox::SharedString* str, game::fox::StringId* id)
		{
			std::lock_guard _0(map_mutex);
			const auto result = shared_string_to_string_id_hook.invoke<game::fox::StringId*>(str, id);
			const auto copy = allocator.duplicate_string(str->data->buffer);
			string_map.insert(std::make_pair(result->id, copy));
			return result;
		}
	}

	const char* lookup(const std::uint64_t id)
	{
		const auto iter = string_map.find(id);
		if (iter != string_map.end())
		{
			return iter->second;
		}

		return "(unknown)";
	}

	const char* lookup(const game::fox::StringId id)
	{
		return lookup(id.id);
	}

	game::fox::StringId compute(const char* str)
	{
		return fox_str_hash_hook.invoke<game::fox::StringId>(str, std::strlen(str));
	}

	game::fox::StringId compute(const std::string& str)
	{
		return compute(str.data());
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{

		}

		void start() override
		{
			fox_str_hash_hook.create(SELECT_VALUE(0x140023260, 0x140022F30, 0x140023220, 0x140022FD0), fox_str_hash_stub);
			shared_string_to_string_id_hook.create(SELECT_VALUE(0x14001D660, 0x14001D4C0, 0x14001D620, 0x14001D490), shared_string_to_string_id_stub);

			command::add("dumpstrings", []
			{
				std::string buffer;
				for (const auto& [k, v] : string_map)
				{
					buffer.append(utils::string::va("0x%llX: %s\n", k, v));
				}

				const auto path = "tpp-mod/strings.txt";
				utils::io::write_file(path, buffer);
				console::info("dumped to %s", path);
			});
		}
	};
}

#ifdef DEBUG
REGISTER_COMPONENT(hashes::component)
#endif
