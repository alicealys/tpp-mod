#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

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

	bool var_value::enabled() const
	{
		return std::get<bool>(this->value_);
	}

	std::int32_t var_value::get_int() const
	{
		return std::get<std::int32_t>(this->value_);
	}

	float var_value::get_float() const
	{
		return std::get<float>(this->value_);
	}

	std::string var_value::get_string() const
	{
		return std::get<std::string>(this->value_);
	}

	var_type_t var_value::get_type() const
	{
		switch (this->value_.index())
		{
		case 1:
			return var_type_boolean;
		case 2:
			return var_type_integer;
		case 3:
			return var_type_float;
		case 4:
			return var_type_string;
		}

		return var_type_none;
	}

	std::optional<var_value> var_value::parse(const std::string& str, const var_type_t type)
	{
		std::string string_value;

		if (str.starts_with("\"") && str.ends_with("\""))
		{
			string_value = str.substr(1, str.size() - 2);
		}
		else
		{
			string_value = str;
		}

		switch (type)
		{
		case var_type_boolean:
			return var_value(std::atoi(string_value.data()) == 1);
		case var_type_integer:
			return var_value(std::atoi(string_value.data()));
		case var_type_float:
			return var_value(static_cast<float>(std::atof(string_value.data())));
		case var_type_string:
			return var_value(string_value);
		}

		return {};
	}

	std::string var_value::to_string() const
	{
		switch (this->get_type())
		{
		case var_type_boolean:
			return utils::string::va("%i", std::get<bool>(this->value_));
		case var_type_integer:
			return utils::string::va("%i", std::get<std::int32_t>(this->value_));
		case var_type_float:
			return utils::string::va("%f", std::get<float>(this->value_));
		case var_type_string:
			return std::get<std::string>(this->value_);
		}

		return "";
	}

	const char* var_value::type_name() const
	{
		switch (this->get_type())
		{
		case var_type_boolean:
			return "bool";
		case var_type_integer:
			return "int";
		case var_type_float:
			return "float";
		case var_type_string:
			return "string";
		}

		return "empty";
	}

	namespace
	{
		bool post_initialization = false;

		std::string get_config_file_path()
		{
			return (utils::properties::get_appdata_path() / "config.cfg").generic_string();
		}

		void write_config()
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			std::string buffer;

			const auto path = get_config_file_path();

			for (const auto& var : get_var_list())
			{
				if ((var->flags & var_flag_saved) == 0)
				{
					continue;
				}

				const auto value = var->current.to_string();
				buffer.append(utils::string::va("set %s \"%s\"\r\n", var->name.data(), value.data()));
			}

			utils::io::write_file(path, buffer, false);
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

		bool check_domain(const var_ptr& var, const var_value& value)
		{
			switch (var->type)
			{
			case var_type_integer:
			{
				const auto value_int = value.get_int();
				return value_int >= var->limits.integer.min && value_int <= var->limits.integer.max;
			}
			case var_type_float:
			{
				const auto value_float = value.get_float();
				return value_float >= var->limits.float_.min && value_float <= var->limits.float_.max;
			}
			}

			return true;
		}

		bool cheats_enabled()
		{
			return utils::flags::has_flag("var-cheat");
		}

		bool check_cheats(const var_ptr& var, const var_source_t set_source)
		{
			return ((var->flags & var_flag_cheat) == 0) || set_source == var_source_internal || cheats_enabled();
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

	void set_var(const var_ptr& var, const var_value& value, const var_source_t set_source)
	{
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

		if (var->type != value.get_type())
		{
			return;
		}

		if (!check_domain(var, value))
		{
			return;
		}

		var->current = value;
		if ((var->flags & var_flag_latched) == 0 || set_source == var_source_internal || !post_initialization)
		{
			var->latched = value;
		}

		if (set_source != var_source_internal)
		{
			var->changed = true;
		}

		if ((var->flags & var_flag_saved) != 0 && set_source != var_source_internal)
		{
			write_config();
		}
	}

	var_ptr register_var(
		const std::string& name, const var_type_t& type, const var_value& value, const var_limits_t limits, const std::uint32_t flags, const std::string& description)
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
			const auto current_str = var->current.to_string();
			const auto latched_str = var->latched.to_string();
			const auto reset_str = var->reset.to_string();

			printf("\"%s\" is: \"%s\" latched: \"%s\" default: \"%s\" type: \"%s\" flags: %i\n",
				var->name.data(), current_str.data(), latched_str.data(), reset_str.data(), var->current.type_name(), var->flags);
			printf("%s\n", var->description.data());
			printf("   %s\n", get_var_domain(var));
			return true;
		}

		const auto value = params.get(1);
		const auto parsed_value = var_value::parse(value, var->type);

		if (parsed_value.has_value())
		{
			set_var(var, parsed_value.value(), var_source_external);
		}

		return true;
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
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
					const auto parsed_value = var_value::parse(value, var.value()->type);
					if (!parsed_value.has_value())
					{
						return;
					}

					set_var(var.value(), parsed_value.value(), var_source_external);
				}
			});

			command::add("var_list", []
			{
				for (const auto& var : get_var_list())
				{
					const auto current_str = var->current.to_string();
					printf("%s \"%s\"\n", var->name.data(), current_str.data());
				}
			});
		}

		void post_load() override
		{

		}

		void start() override
		{

		}

		void post_start() override
		{
			post_initialization = true;
		}
	};
}

REGISTER_COMPONENT(vars::component)
