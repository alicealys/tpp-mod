#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "filesystem.hpp"
#include "console.hpp"
#include "vars.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/cryptography.hpp>
#include <utils/flags.hpp>

namespace plugins
{
	namespace
	{
		struct plugin_handle_t
		{
			std::string name;
			HMODULE module;
		};

		std::vector<plugin_handle_t> loaded_plugins;
		vars::var_ptr fs_load_plugins;
	}

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
			console::info("[plugins] Loading plugin \"%s\"...\n", name.data());

			const auto data = utils::io::read_file(file);
			const auto hash = utils::cryptography::md5::compute(data, true);
			const auto hash_short = hash.substr(0, 8);

			const auto new_name = std::format("__plugin{}.dll", hash_short);
			utils::io::write_file(new_name, data);
			utils::io::hide_file(new_name);

			loaded_plugins.emplace_back(name, utils::nt::library::load(new_name));
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			fs_load_plugins = vars::register_bool("fs_load_plugins", true, vars::var_flag_saved, "enable plugin loading");

			delete_old_plugins();
		}

		void start() override
		{
			if (!fs_load_plugins->current.enabled())
			{
				return;
			}

			const auto paths = filesystem::get_search_paths();
			for (const auto& path : paths)
			{
				load_plugins(path + "/plugins");
			}
		}

		void end() override
		{
			for (const auto& plugin : loaded_plugins)
			{
				console::info("[plugins] Unloading plugin \"%s\"...\n", plugin.name.data());
				FreeLibrary(plugin.module);
			}

			loaded_plugins.clear();
		}
	};
}

REGISTER_COMPONENT(plugins::component)
