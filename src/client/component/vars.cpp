#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "vars.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/properties.hpp>
#include <utils/string.hpp>
#include <utils/flags.hpp>

namespace vars
{
	std::vector<var_ptr>& get_var_list()
	{
		static std::vector<var_ptr> vars;
		return vars;
	}

	std::unordered_map<std::string, var_ptr>& get_var_map()
	{
		static std::unordered_map<std::string, var_ptr> vars;
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
			for (const auto& var : get_var_list())
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
			if (var->type == var_type_string)
			{
				return true;
			}

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
			{
				if (value.is_string())
				{
					return value.get<std::string>();
				}
				else
				{
					return value.dump();
				}
			}
			}

			throw std::runtime_error("invalid cast type");
		}

		bool cheats_enabled()
		{
			return utils::flags::has_flag("var-cheat");
		}

		bool check_cheats(const var_ptr& var, const var_source_t set_source)
		{
			return ((var->flags & var_flag_cheat) == 0) || set_source == var_source_internal || cheats_enabled();
		}

		void set_var(const var_ptr& var, const nlohmann::json& value, const var_source_t set_source)
		{
			const auto set_value = [&](const nlohmann::json& value)
			{
				const auto casted_value = cast_value(var, value);

				var->current = casted_value;
				if ((var->flags & var_flag_latched) == 0 || set_source == var_source_internal)
				{
					var->latched = casted_value;
				}

				if ((var->flags & var_flag_saved) != 0 && set_source != var_source_internal)
				{
					write_config();
				}
			};

			if (!check_cheats(var, set_source))
			{
				console::warn("\"%s\" is cheat protected", var->name.data());
				return;
			}

			if ((var->flags & var_flag_readonly) != 0 && set_source != var_source_internal)
			{
				console::warn("\"%s\" is read only", var->name.data());
				return;
			}

			if (value.is_null())
			{
				set_value(var->reset);
				return;
			}

			if (!check_type(var, value))
			{
				console::warn("[var] invalid value type \"%s\" for var \"%s\"\n", value.type_name(), var->name.data());
				return;
			}

			if (!check_domain(var, value))
			{
				const auto value_str = value.dump();
				console::warn("[var] invalid value %s for var \"%s\"\n", value_str.data(), var->name.data());
				return;
			}

			set_value(value);
		}

		void parse_saved_vars()
		{
			const auto cfg = read_config();

			for (const auto& var : get_var_list())
			{
				if ((var->flags & var_flag_saved) == 0 || !cfg.contains(var->name))
				{
					continue;
				}

				const auto& value = cfg.at(var->name);
				set_var(var, value, var_source_internal);
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
			if (value_j.is_discarded() || !value_j.is_primitive())
			{
				return text;
			}

			return value_j;
		}

		std::string var_value_to_string(const var_ptr& var, const nlohmann::json& value)
		{
			return var->type == var_type_string ? value.get<std::string>() : value.dump();
		}
	}

	var_ptr register_var(
		const std::string& name, const var_type_t& type, const var_value_t& value, const var_limits_t limits, const std::uint32_t flags, const std::string& description)
	{
		const auto lower = utils::string::to_lower(name);

		auto var = std::make_shared<var_t>();
		var->name = lower;
		var->description = description;

		var->type = type;
		var->flags = flags;

		var->current = value;
		var->latched = value;
		var->reset = value;
		var->limits = limits;

		get_var_list().emplace_back(var);
		get_var_map().insert(std::make_pair(lower, var));

		return var;
	}

	var_ptr register_bool(const std::string& name, bool value, const std::uint32_t flags, const std::string& description)
	{
		return register_var(name, var_type_boolean, value, {}, flags, description);
	}

	var_ptr register_int(const std::string& name, std::int32_t value, std::int32_t min, std::int32_t max,
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

	var_ptr register_float(const std::string& name, float value, float min, float max,
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

	var_ptr register_string(const std::string& name, const std::string& value,
		const std::uint32_t flags, const std::string& description)
	{
		return register_var(name, var_type_string, value, {}, flags, description);
	}

	std::optional<var_ptr> find(const std::string& name)
	{
		const auto lower = utils::string::to_lower(name);
		const auto& map = get_var_map();
		const auto iter = map.find(lower);

		if (iter == map.end())
		{
			return {};
		}

		return {iter->second};
	}

	std::optional<std::string> find_name(const std::string& name)
	{
		const auto lower = utils::string::to_lower(name);

		for (const auto& var : get_var_list())
		{
			if (var->name.starts_with(lower))
			{
				return {var->name};
			}
		}

		return {};
	}

	bool var_command(const command::params& params)
	{
		const auto name = params.get(0);
		const auto var_opt = find(name);

		if (!var_opt.has_value())
		{
			return false;
		}

		auto& var = var_opt.value();

		if (params.size() == 1)
		{
			const auto current_str = var_value_to_string(var, var->current);
			const auto latched_str = var_value_to_string(var, var->latched);
			const auto reset_str = var_value_to_string(var, var->reset);

			printf("\"%s\" is: \"%s\" latched: \"%s\" default: \"%s\" type: \"%s\" flags: %i\n",
				var->name.data(), current_str.data(), latched_str.data(), reset_str.data(), var->current.type_name(), var->flags);
			printf("%s\n", var->description.data());
			printf("   %s\n", get_var_domain(var));
			return true;
		}

		const auto value = params.get(1);
		const auto parsed_value = parse_value_text(value);

		set_var(var, parsed_value, var_source_external);
		return true;
	}

	class component final : public component_interface
	{
	public:

		void pre_load() override
		{
		}

		void post_load() override
		{
			parse_saved_vars();
		}

		void start() override
		{
			command::add("set", [](const command::params& params)
			{
				if (params.size() < 3)
				{
					return;
				}

				const auto name = params.get(1);
				const auto value = params.get(2);

				const auto var = find(name);
				if (!var.has_value())
				{
					register_string(name, value, var_flag_external, "");
				}
				else
				{
					const auto parsed_value = parse_value_text(value);
					set_var(var.value(), parsed_value, var_source_external);
				}
			});

			command::add("var_list", []
			{
				for (const auto& var : get_var_list())
				{
					const auto current_str = var_value_to_string(var, var->current);
					printf("\"%s\": \"%s\"\n", var->name.data(), current_str.data());
				}
			});
		}
	};
}

REGISTER_COMPONENT(vars::component)
