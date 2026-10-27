#include <std_include.hpp>

#include "fox.hpp"

namespace utils::fox::fs
{
	path::path(const std::string& data)
	{
		game::fox::fs::Path_::Path_(&this->path_, data.data());
	}

	path::~path()
	{
		game::fox::fs::Path_::Path__destructor(&this->path_);
	}

	game::fox::fs::Path* path::get_instance()
	{
		return &this->path_;
	}

	stream::stream()
	{
		game::fox::fs::Stream_::Stream_(&this->stream_);
	}

	bool stream::open(const std::string& path_str)
	{
		path path(path_str);
		auto result = 0;
		game::fox::fs::Stream_::Open(&this->stream_, &result, path.get_instance(), 1);
		return result >= 0;
	}

	bool stream::read(std::string& data)
	{
		const auto size = game::fox::fs::Stream_::GetSize(&this->stream_, 0);
		if (size == 0ull)
		{
			return false;
		}

		data.resize(size);
		auto result = 0;
		game::fox::fs::Stream_::Read(&this->stream_, &result, data.data(), data.size(), 0);
		return true;
	}

	stream::~stream()
	{
		auto result = 0;
		game::fox::fs::Stream_::Close(&this->stream_, &result);
	}

	bool read_file(const std::string& path, std::string& data)
	{
		stream stream;
		return stream.open(path) && stream.read(data);
	}
}
