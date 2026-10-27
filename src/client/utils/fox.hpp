#pragma once

#include "game/game.hpp"

namespace utils::fox::fs
{
	class path
	{
	public:
		path(const std::string& data);
		~path();

		game::fox::fs::Path* get_instance();

	private:
		game::fox::fs::Path path_{};

	};

	class stream
	{
	public:
		stream();
		~stream();

		bool open(const std::string& path_str);
		bool read(std::string& data);
		void close();

	private:
		game::fox::fs::Stream stream_{};

	};

	bool read_file(const std::string& path, std::string& data);
}
