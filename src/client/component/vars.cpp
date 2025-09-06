#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "vars.hpp"
#include "console.hpp"
#include "command.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/properties.hpp>
#include <utils/string.hpp>

namespace vars
{
	std::vector<var_ptr>& get_vars()
	{
		static std::vector<var_ptr> vars;
		return vars;
	}

	namespace
	{
		std::string get_config_file_path()
		{
			return (utils::properties::get_appdata_path() / "config.json").generic_string();
		}

		void write_config()
		{
			const auto path = get_config_file_path();

			nlohmann::json config;
			for (const auto& var : get_vars())
			{
				if ((var->flags & var_flag_saved) == 0)
				{
					continue;
				}

				config[var->name] = var->current;
			}

			const auto str = config.dump(4);
			utils::io::write_file(path, str, false);
		}

		nlohmann::json read_config()
		{
			const auto path = get_config_file_path();
			if (!utils::io::file_exists(path))
			{
				return {};
			}

			try
			{
				const auto data = utils::io::read_file(path);
				return nlohmann::json::parse(data);
			}
			catch (const std::exception& e)
			{
				console::error("Failed to parse config file: %s\n", e.what());
				utils::io::write_file(path, "{}", false);
			}

			return {};
		}

		bool check_domain(const var_ptr& var, const nlohmann::json& value)
		{
			switch (var->type)
			{
			case var_type_integer:
			{
				const auto value_int = value.get<std::int32_t>();
				return value_int >= var->limits.integer.min && value_int < var->limits.integer.max;
			}
			case var_type_float:
			{
				const auto value_float = value.get<std::int32_t>();
				return value_float >= var->limits.float_.min && value_float < var->limits.float_.max;
			}
			}

			return true;
		}

		bool check_type(const var_ptr& var, const nlohmann::json& value)
		{
			const auto type = value.type();

			switch (type)
			{
			case nlohmann::json::value_t::number_integer:
			case nlohmann::json::value_t::number_unsigned:
				return var->type == var_type_integer || var->type == var_type_float || var->type == var_type_boolean;
			case nlohmann::json::value_t::number_float:
				return var->type == var_type_float;
			case nlohmann::json::value_t::string:
				return var->type == var_type_string;
			case nlohmann::json::value_t::boolean:
				return var->type == var_type_boolean;
			}

			return false;
		}

		nlohmann::json cast_value(const var_ptr& var, const nlohmann::json& value)
		{
			switch (var->type)
			{
			case var_type_integer:
				return value.get<std::int32_t>();
			case var_type_float:
				if (value.is_number_integer())
				{
					return static_cast<float>(value.get<std::int32_t>());
				}

				return value.get<float>();
			case var_type_boolean:
			{
				if (value.is_number_integer())
				{
					return value.get<std::int32_t>() == 1;
				}

				return value.get<bool>();
			}
			case var_type_string:
				return value.get<std::string>();
			}

			throw std::runtime_error("invalid cast type");
		}

		void set_var(const var_ptr& var, const nlohmann::json& value, bool force)
		{
			const auto set_value = [&](const nlohmann::json& value)
			{
				const auto casted_value = cast_value(var, value);

				var->current = casted_value;
				if ((var->flags & var_flag_latched) == 0 || force)
				{
					var->latched = casted_value;
				}

				if ((var->flags & var_flag_saved) != 0)
				{
					write_config();
				}
			};

			if (value.is_null())
			{
				set_value(var->reset);
				return;
			}

			if (!check_type(var, value))
			{
				printf("[var] invalid value type \"%s\" for var \"%s\"\n", value.type_name(), var->name.data());
				return;
			}

			if (!check_domain(var, value))
			{
				const auto value_str = value.dump();
				printf("[var] invalid value %s for var \"%s\"\n", value_str.data(), var->name.data());
				return;
			}

			set_value(value);
		}

		void parse_saved_vars()
		{
			const auto cfg = read_config();

			for (const auto& var : get_vars())
			{
				if ((var->flags & var_flag_saved) == 0 || !cfg.contains(var->name))
				{
					continue;
				}

				const auto& value = cfg.at(var->name);
				set_var(var, value, true);
			}
		}

		const char* get_var_domain(const var_ptr& var)
		{
			switch (var->type)
			{
			case var_type_boolean:
				return "domain is 0 or 1";
			case var_type_float:
			{
				if (var->limits.float_.min == std::numeric_limits<float>::min())
				{
					if (var->limits.float_.max == std::numeric_limits<float>::max())
					{
						return utils::string::va("domain is any number");
					}
					else
					{
						return utils::string::va("domain is any number %f or smaller", var->limits.float_.max);
					}
				}
				else
				{
					if (var->limits.float_.max == std::numeric_limits<float>::max())
					{
						return utils::string::va("domain is any number %f or bigger", var->limits.float_.min);
					}
					else
					{
						return utils::string::va("domain is any number %f to %f", var->limits.float_.min, var->limits.float_.max);
					}
				}
			}
			case var_type_integer:
			{
				if (var->limits.integer.min == std::numeric_limits<std::int32_t>::min())
				{
					if (var->limits.integer.max == std::numeric_limits<std::int32_t>::max())
					{
						return utils::string::va("domain is any integer");
					}
					else
					{
						return utils::string::va("domain is any integer %i or smaller", var->limits.integer.max);
					}
				}
				else
				{
					if (var->limits.integer.max == std::numeric_limits<std::int32_t>::max())
					{
						return utils::string::va("domain is any integer %i or bigger", var->limits.integer.min);
					}
					else
					{
						return utils::string::va("domain is any integer %i to %i", var->limits.integer.min, var->limits.integer.max);
					}
				}
			}
			case var_type_string:
				return "domain is any text";
			}

			return "";
		}

		nlohmann::json parse_value_text(const std::string& text)
		{
			const auto value_j = nlohmann::json::parse(text, nullptr, false);
			if (value_j.is_discarded())
			{
				return text;
			}

			return value_j;
		}
	}

	var_ptr& register_var(
		const std::string& name, const var_type_t& type, const var_value_t& value, const var_limits_t limits, const std::uint32_t flags, const std::string& description)
	{
		auto var = std::make_unique<var_t>();
		var->name = name;
		var->description = description;

		var->type = type;
		var->flags = flags;

		var->current = value;
		var->latched = value;
		var->reset = value;
		var->limits = limits;

		return get_vars().emplace_back(std::move(var));
	}

	var_ptr& register_bool(const std::string& name, bool value, const std::uint32_t flags, const std::string& description)
	{
		return register_var(name, var_type_boolean, value, {}, flags, description);
	}

	var_ptr& register_int(const std::string& name, std::int32_t value, std::int32_t min, std::int32_t max,
		const std::uint32_t flags, const std::string& description)
	{
		var_limits_t limits{};
		limits.integer.min = min;
		limits.integer.max = max;

		if (min == max)
		{
			limits.integer.min = std::numeric_limits<std::int32_t>::min();
			limits.integer.max = std::numeric_limits<std::int32_t>::max();
		}

		return register_var(name, var_type_integer, value, limits, flags, description);
	}

	var_ptr& register_float(const std::string& name, float value, float min, float max,
		const std::uint32_t flags, const std::string& description)
	{
		var_limits_t limits{};
		limits.float_.min = min;
		limits.float_.max = max;

		if (min == max)
		{
			limits.float_.min = std::numeric_limits<float>::min();
			limits.float_.max = std::numeric_limits<float>::max();
		}

		return register_var(name, var_type_float, value, limits, flags, description);
	}

	var_ptr& register_string(const std::string& name, float value, float min, float max,
		const std::uint32_t flags, const std::string& description)
	{
		return register_var(name, var_type_string, value, {}, flags, description);
	}

	class component final : public component_interface
	{
	public:

		void post_start() override
		{
		}

		void post_load() override
		{
			parse_saved_vars();
		}

		void post_unpack() override
		{
			for (const auto& var : get_vars())
			{
				command::add(var->name, [&](const command::params& params)
				{
					if (params.size() == 1)
					{
						const auto current_str = var->current.dump();
						const auto latched_str = var->latched.dump();
						const auto reset_str = var->reset.dump();

						printf("\"%s\" is: \"%s\" latched: \"%s\" default: \"%s\" type: \"%s\" flags: %i\n", 
							var->name.data(), current_str.data(), latched_str.data(), reset_str.data(), var->current.type_name(), var->flags);
						printf("%s\n", var->description.data());
						printf("   %s\n", get_var_domain(var));
						return;
					}

					const auto value = params.get(1);
					const auto parsed_value = parse_value_text(value);

					set_var(var, parsed_value, false);
				});
			}

			command::add("var_list", []
			{
				for (const auto& var : get_vars())
				{
					const auto current_str = var->current.dump();
					printf("\"%s\": \"%s\"\n", var->name.data(), current_str.data());
				}
			});
		}
	};
}

REGISTER_COMPONENT(vars::component)
