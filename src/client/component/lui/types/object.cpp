#include <std_include.hpp>

#include "object.hpp"

namespace lui
{
	// bool

	object_value::object_value(const bool value)
	{
		this->value_ = value;
	}

	template <>
	bool object_value::is<bool>() const
	{
		return this->value_.index() == VALUE_TYPE_BOOL;
	}

	template <>
	bool object_value::get() const
	{
		return std::get<bool>(this->value_);
	}

	// integer

	object_value::object_value(const std::int32_t value)
	{
		integer_value_t int_value{};
		int_value.i32 = value;
		this->value_ = int_value;
	}

	template <>
	bool object_value::is<std::int32_t>() const
	{
		return this->value_.index() == VALUE_TYPE_INTEGER;
	}

	template <>
	std::int32_t object_value::get() const
	{
		return std::get<integer_value_t>(this->value_).i32;
	}

	object_value::object_value(const std::uint32_t value)
	{
		integer_value_t int_value{};
		int_value.u32 = value;
		this->value_ = int_value;
	}

	template <>
	bool object_value::is<std::uint32_t>() const
	{
		return this->value_.index() == VALUE_TYPE_INTEGER;
	}

	template <>
	std::uint32_t object_value::get() const
	{
		return std::get<integer_value_t>(this->value_).u32;
	}

	object_value::object_value(const std::int64_t value)
	{
		integer_value_t int_value{};
		int_value.i64 = value;
		this->value_ = int_value;
	}

	template <>
	bool object_value::is<std::int64_t>() const
	{
		return this->value_.index() == VALUE_TYPE_INTEGER;
	}

	template <>
	std::int64_t object_value::get() const
	{
		return std::get<integer_value_t>(this->value_).i64;
	}

	object_value::object_value(const std::uint64_t value)
	{
		integer_value_t int_value{};
		int_value.u64 = value;
		this->value_ = int_value;
	}

	template <>
	bool object_value::is<std::uint64_t>() const
	{
		return this->value_.index() == VALUE_TYPE_INTEGER;
	}

	template <>
	std::uint64_t object_value::get() const
	{
		return std::get<integer_value_t>(this->value_).u64;
	}

	// float

	object_value::object_value(const float value)
	{
		this->value_ = value;
	}

	template <>
	bool object_value::is<float>() const
	{
		return this->value_.index() == VALUE_TYPE_FLOAT;
	}

	template <>
	float object_value::get() const
	{
		return std::get<float>(this->value_);
	}

	// string

	object_value::object_value(const char* value)
	{
		this->value_ = value;
	}

	object_value::object_value(const std::string& value)
	{
		this->value_ = value;
	}

	template <>
	bool object_value::is<std::string>() const
	{
		return this->value_.index() == VALUE_TYPE_STRING;
	}

	template <>
	std::string object_value::get() const
	{
		return std::get<std::string>(this->value_);
	}

	//

	object_value::type_t object_value::get_type() const
	{
		return static_cast<object_value::type_t>(this->value_.index());
	}

	bool object_value::is_null() const
	{
		return this->value_.index() == VALUE_TYPE_NONE;
	}

	bool object_value::operator==(const object_value& value)
	{
		return false;
	}

	const object_base& object::get_values() const
	{
		return this->values_;
	}
}
