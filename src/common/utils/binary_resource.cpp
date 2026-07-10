#include "binary_resource.hpp"

#include <utility>
#include "nt.hpp"
#include "io.hpp"

namespace utils
{
	binary_resource::binary_resource(const int id, std::string file)
		: filename_(std::move(file))
	{
		this->resource_ = nt::load_resource(id);

		if (this->resource_.empty())
		{
			throw std::runtime_error("Unable to load resource: " + std::to_string(id));
		}
	}

	std::string binary_resource::get_extracted_file(const bool fatal_if_overwrite_fails)
	{
		if (this->path_.empty())
		{
			this->path_ = nt::write_exitisting_temp_file(this->filename_, this->resource_, fatal_if_overwrite_fails);
		}

		return this->path_;
	}

	const std::string& binary_resource::get_data() const
	{
		return this->resource_;
	}
}
