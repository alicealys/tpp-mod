#pragma once

#include "command.hpp"

namespace vars
{
	enum var_type_t
	{
		var_type_none = 0,
		var_type_boolean = 1,
		var_type_integer = 2,
		var_type_float = 3,
		var_type_string = 4,
		var_type_vec2 = 5,
		var_type_vec3 = 6,
		var_type_vec4 = 7,
		var_type_color = 8,
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

	struct vec2_t
	{
		float x;
		float y;
	};

	struct vec3_t
	{
		float x;
		float y;
		float z;
	};

	struct vec4_t
	{
		float x;
		float y;
		float z;
		float w;
	};

	struct color_t
	{
		float r;
		float g;
		float b;
		float a;
	};

	using var_value_variant_t = std::variant<
		std::monostate, 
		bool, 
		std::int32_t, 
		float, 
		std::string, 
		vec2_t,
		vec3_t,
		vec4_t,
		color_t
	>;

	class var_value
	{
	public:
		var_value() = default;

		template <typename T>
		var_value(const T& value)
		{
			this->value_ = value;
		}

		bool enabled() const;
		std::int32_t get_int() const;
		float get_float() const;
		std::string get_string() const;

		vec2_t get_vec2() const;
		vec3_t get_vec3() const;
		vec4_t get_vec4() const;
		color_t get_color() const;

		std::string to_string() const;
		const char* type_name() const;
		var_type_t get_type() const;

		static std::optional<var_value> parse(const std::string& value, const var_type_t type);

	private:
		var_value_variant_t value_{};

	};

	class var_t
	{
	public:
		std::string name;
		std::string description;
		std::uint32_t flags;
		bool changed;
		var_value current;
		var_value latched;
		var_value reset;
		var_type_t type;
		var_limits_t limits;
	};

	using var_ptr = std::shared_ptr<var_t>;

	void set_var(const var_ptr& var, const var_value& value, const var_source_t set_source);

	var_ptr register_var(
		const std::string& name, const var_type_t& type, const var_value& value, const var_limits_t limits, const std::uint32_t flags, const std::string& description);

	var_ptr register_bool(const std::string& name, bool value, const std::uint32_t flags, const std::string& description);

	var_ptr register_int(const std::string& name, std::int32_t value, std::int32_t min, std::int32_t max,
		const std::uint32_t flags, const std::string& description);

	var_ptr register_float(const std::string& name, float value, float min, float max,
		const std::uint32_t flags, const std::string& description);

	var_ptr register_string(const std::string& name, const std::string& value,
		const std::uint32_t flags, const std::string& description);

	var_ptr register_vec2(const std::string& name, const vec2_t& value, 
		const std::uint32_t flags, const std::string& description);

	var_ptr register_vec3(const std::string& name, const vec3_t& value,
		const std::uint32_t flags, const std::string& description);

	var_ptr register_vec4(const std::string& name, const vec4_t& value,
		const std::uint32_t flags, const std::string& description);

	var_ptr register_color(const std::string& name, const color_t& value,
		const std::uint32_t flags, const std::string& description);

	std::optional<var_ptr> find(const std::string& name);
	std::optional<std::string> find_name(const std::string& name);

	bool var_command(const command::params& params);
}
