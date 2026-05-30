#pragma once

namespace lui
{
	class object_value
	{
	public:
		enum type_t
		{
			VALUE_TYPE_NONE = 0,
			VALUE_TYPE_BOOL = 1,
			VALUE_TYPE_INTEGER = 2,
			VALUE_TYPE_FLOAT = 3,
			VALUE_TYPE_STRING = 4
		};

		union integer_value_t
		{
			std::int32_t i32;
			std::uint32_t u32;
			std::int64_t i64;
			std::uint64_t u64;
		};

		using variant_t = std::variant<
			std::monostate,
			bool,
			integer_value_t,
			float,
			std::string
		>;

		object_value() = default;

		object_value(const bool value);
		object_value(const std::int32_t value);
		object_value(const std::uint32_t value);
		object_value(const std::int64_t value);
		object_value(const std::uint64_t value);
		object_value(const float value);
		object_value(const char* value);
		object_value(const std::string& value);

		template <typename T>
		bool is() const;

		template <typename T>
		T as() const
		{
			if (!this->is<T>())
			{
				throw std::runtime_error("invalid type");
			}

			return this->get<T>();
		}

		bool is_null() const;

		type_t get_type() const;

	private:
		template <typename T>
		T get() const;

		variant_t value_;

	};

	using object_base = std::unordered_map<std::string, object_value>;
	class object : public object_base
	{
	public:
		object() = default;

		template <typename T>
		T get_or(const std::string& key, const T& or_value) const
		{
			const auto iter = this->find(key);
			if (iter == this->end())
			{
				return or_value;
			}

			if (!iter->second.is<T>())
			{
				return or_value;
			}

			return iter->second.get<T>();
		}

		template <typename T>
		T get(const std::string& key) const
		{
			const auto iter = this->find(key);
			if (iter == this->end())
			{
				throw std::runtime_error("bad access");
			}

			return iter->second.as<T>();
		}

		template <typename T>
		void set(const std::string& key, const T& value)
		{
			this->operator[](key) = object_value(value);
		}

	};
}
