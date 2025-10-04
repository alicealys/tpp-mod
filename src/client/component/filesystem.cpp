#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "filesystem.hpp"
#include "console.hpp"

#include "game/game.hpp"

#include <utils/io.hpp>
#include <utils/hook.hpp>
#include <utils/flags.hpp>
#include <utils/properties.hpp>

namespace filesystem
{
	namespace
	{
		struct resource_file_t
		{
			std::string path;
			int resource_id;
		};

		std::unordered_map<std::string, int>& get_resource_files()
		{
			static std::unordered_map<std::string, int> resource_files{};
			return resource_files;
		}

		std::deque<std::filesystem::path>& get_search_paths_internal()
		{
			static std::deque<std::filesystem::path> search_paths{};
			return search_paths;
		}

		std::vector<std::filesystem::path> get_paths(const std::filesystem::path& path)
		{
			std::vector<std::filesystem::path> paths{};

			paths.push_back(path);

			return paths;
		}

		bool can_insert_path(const std::filesystem::path& path)
		{
			const auto& paths = get_search_paths_internal();
			return std::ranges::none_of(paths.cbegin(), paths.cend(), [path](const auto& elem)
			{
				return elem == path;
			});
		}

		bool is_parent_path(const std::filesystem::path& parent, const std::filesystem::path& child)
		{
			std::filesystem::path iter = child;

			while (iter != iter.parent_path())
			{
				if (iter == parent)
				{
					return true;
				}

				iter = iter.parent_path();
			}

			return false;
		}

		bool read_file_internal(const std::string& path, std::string* data = nullptr, std::string* real_path = nullptr)
		{
			const auto set_result = [&](const std::string& data_, const std::string& real_path_)
			{
				if (data != nullptr)
				{
					*data = data_;
				}

				if (real_path != nullptr)
				{
					*real_path = real_path_;
				}
			};

			for (const auto& search_path : get_search_paths_internal())
			{
				const auto full_path = (search_path / path).generic_string();
				if (utils::io::file_exists(full_path))
				{
					set_result(utils::io::read_file(full_path), full_path);
					return true;
				}
			}

			auto& resource_files = get_resource_files();
			const auto iter = resource_files.find(path);
			if (iter == resource_files.end())
			{
				return false;
			}

			set_result(utils::nt::load_resource(iter->second), "");
			return true;
		}
	}

	std::string read_file(const std::string& path)
	{
		std::string data;
		read_file_internal(path, &data);
		return data;
	}

	bool read_file(const std::string& path, std::string* data, std::string* real_path)
	{
		return read_file_internal(path, data, real_path);
	}

	bool find_file(const std::string& path, std::string* real_path)
	{
		std::string data;
		return read_file_internal(path, &data, real_path);
	}

	bool exists(const std::string& path)
	{
		return read_file_internal(path);
	}

	void register_path(const std::filesystem::path& path)
	{
		const auto paths = get_paths(path);
		for (const auto& path_ : paths)
		{
			if (can_insert_path(path_))
			{
				console::info("[FS] Registering path '%s'\n", path_.generic_string().data());
				get_search_paths_internal().push_front(path_);
			}
		}
	}

	void unregister_path(const std::filesystem::path& path)
	{
		const auto paths = get_paths(path);
		for (const auto& path_ : paths)
		{
			auto& search_paths = get_search_paths_internal();
			for (auto i = search_paths.begin(); i != search_paths.end();)
			{
				if (*i == path_)
				{
					console::info("[FS] Unregistering path '%s'\n", path_.generic_string().data());
					i = search_paths.erase(i);
				}
				else
				{
					++i;
				}
			}
		}
	}

	void register_resource_file(const std::string& path, const int resource_id)
	{
		get_resource_files()[path] = resource_id;
	}

	std::vector<std::string> get_search_paths()
	{
		std::vector<std::string> paths{};

		for (const auto& path : get_search_paths_internal())
		{
			paths.push_back(path.generic_string());
		}

		return paths;
	}

	std::vector<std::string> get_search_paths_rev()
	{
		std::vector<std::string> paths{};
		const auto& search_paths = get_search_paths_internal();

		for (auto i = search_paths.rbegin(); i != search_paths.rend(); ++i)
		{
			paths.push_back(i->generic_string());
		}

		return paths;
	}

	void check_path(const std::filesystem::path& path)
	{
		if (path.generic_string().find("..") != std::string::npos)
		{
			throw std::runtime_error("directory traversal is not allowed");
		}
	}

	std::string get_safe_path(const std::filesystem::path& path)
	{
		check_path(path);
		const auto absolute = std::filesystem::weakly_canonical(path);

		static std::vector<std::filesystem::path> allowed_directories =
		{
			{std::filesystem::weakly_canonical("tpp-mod")},
		};

		auto is_allowed = false;
		for (const auto& dir : allowed_directories)
		{
			if (is_parent_path(dir, absolute))
			{
				is_allowed = true;
				break;
			}
		}

		if (!is_allowed)
		{
			throw std::runtime_error(std::format("Disallowed access to directory \"{}\"", path.generic_string()));
		}

		return path.generic_string();
	}

	bool safe_write_file(const std::string& file, const std::string& data, bool append)
	{
		const auto path = filesystem::get_safe_path(file);
		return utils::io::write_file(path, data, append);
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			filesystem::register_path(L".");
			filesystem::register_path(L"tpp-mod");
			filesystem::register_path(utils::properties::get_appdata_path());
		}
	};
}

REGISTER_COMPONENT(filesystem::component)
