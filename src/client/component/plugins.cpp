#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "filesystem.hpp"
#include "console.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/cryptography.hpp>

namespace plugins
{
	void delete_old_plugins()
	{
		const auto files = utils::io::list_files(".");
		for (const auto& file : files)
		{
			const auto name = file.substr(file.find_last_of("\\/") + 1);
			if (name.starts_with("__plugin") && name.ends_with(".dll"))
			{
				utils::io::remove_file(file);
			}
		}
	}

	void load_plugins(const std::string& dir)
	{
		if (!utils::io::directory_exists(dir))
		{
			return;
		}

		const auto files = utils::io::list_files(dir);
		for (const auto& file : files)
		{
			if (!file.ends_with(".dll"))
			{
				continue;
			}

			const auto name = file.substr(file.find_last_of("\\/") + 1);
			console::info("Loading plugin \"%s\"...\n", name.data());

			const auto data = utils::io::read_file(file);
			const auto hash = utils::cryptography::md5::compute(data, true);
			const auto hash_short = hash.substr(0, 8);

			const auto new_name = std::format("__plugin{}.dll", hash_short);
			utils::io::write_file(new_name, data);
			utils::io::hide_file(new_name);

			utils::nt::library::load(new_name);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			delete_old_plugins();

			const auto paths = filesystem::get_search_paths();
			for (const auto& path : paths)
			{
				load_plugins(path + "/plugins");
			}
		}
	};
}

REGISTER_COMPONENT(plugins::component)
