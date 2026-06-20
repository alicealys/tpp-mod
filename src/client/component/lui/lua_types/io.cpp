#include <std_include.hpp>

#include "io.hpp"
#include "../../filesystem.hpp"

#include <utils/string.hpp>
#include <utils/io.hpp>

namespace lui::scripting::io
{
	void setup(sol::state& state)
	{
		state["io"] = state.create_table();
		state["io"]["fileexists"] = filesystem::safe_io_func<bool>(utils::io::file_exists);
		state["io"]["writefile"] = filesystem::safe_write_file;
		state["io"]["filesize"] = filesystem::safe_io_func<size_t>(utils::io::file_size);
		state["io"]["createdirectory"] = filesystem::safe_io_func<bool>(utils::io::create_directory);
		state["io"]["directoryexists"] = filesystem::safe_io_func<bool>(utils::io::directory_exists);
		state["io"]["directoryisempty"] = filesystem::safe_io_func<bool>(utils::io::directory_is_empty);
		state["io"]["listfiles"] = filesystem::safe_io_func<std::vector<std::string>>(utils::io::list_files);
		state["io"]["removefile"] = filesystem::safe_io_func<bool>(utils::io::remove_file);
		state["io"]["removedirectory"] = filesystem::safe_io_func<bool>(utils::io::remove_directory);
		state["io"]["readfile"] = filesystem::safe_io_func<std::string>(
			static_cast<std::string(*)(const std::string&)>(utils::io::read_file));
	}
}
