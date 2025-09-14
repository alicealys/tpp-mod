#pragma once

#include "command.hpp"

namespace vars
{
	using var_value_t = nlohmann::json;

	using validate_callback_t = std::function<bool(const var_value_t&)>;

	enum var_type_t
	{
		var_type_none = 0,
		var_type_boolean = 1,
		var_type_integer = 2,
		var_type_float = 3,
		var_type_string = 4,
	};

	enum var_flags_t
	{
		var_flag_saved = 1 << 0,
		var_flag_latched = 1 << 1,
		var_flag_cheat = 1 << 2,
		var_flag_external = 1 << 3,
		var_flag_readonly = 1 << 4,
	};

	enum var_source_t
	{
		var_source_internal = 0,
		var_source_external = 1,
	};

	struct var_limits_int_t
	{
		int min;
		int max;
	};

	struct var_limits_float_t
	{
		float min;
		float max;
	};

	union var_limits_t
	{
		var_limits_int_t integer;
		var_limits_float_t float_;
	};

	struct var_t
	{
		std::string name;
		std::string description;
		std::uint32_t flags;
		var_value_t current;
		var_value_t latched;
		var_value_t reset;
		var_type_t type;
		var_limits_t limits;
	};

	using var_ptr = std::shared_ptr<var_t>;

	var_ptr register_var(
		const std::string& name, const var_type_t& type, const var_value_t& value, const var_limits_t limits, const std::uint32_t flags, const std::string& description);

	var_ptr register_bool(const std::string& name, bool value, const std::uint32_t flags, const std::string& description);

	var_ptr register_int(const std::string& name, std::int32_t value, std::int32_t min, std::int32_t max,
		const std::uint32_t flags, const std::string& description);

	var_ptr register_float(const std::string& name, float value, float min, float max,
		const std::uint32_t flags, const std::string& description);

	var_ptr register_string(const std::string& name, const std::string& value,
		const std::uint32_t flags, const std::string& description);

	std::optional<var_ptr> find(const std::string& name);
	std::optional<std::string> find_name(const std::string& name);

	bool var_command(const command::params& params);
}
