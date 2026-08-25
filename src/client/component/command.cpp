#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "console.hpp"
#include "scheduler.hpp"
#include "vars.hpp"
#include "filesystem.hpp"
#include "binds.hpp"
#include "scripting.hpp"

#include <utils/io.hpp>
#include <utils/string.hpp>
#include <utils/concurrency.hpp>

namespace command
{
	namespace
	{
		std::unordered_map<std::string, callback> commands;
		std::unordered_map<std::string, std::string> aliases;

		std::mutex queue_mutex;

		struct command_queue_entry_t
		{
			std::string cmd;
			std::uint32_t wait;
		};

		using command_queue_t = std::vector<command_queue_entry_t>;

		utils::concurrency::container<command_queue_t> next_command_queue;
		utils::concurrency::container<command_queue_t> command_queue;

		void parse_command_line()
		{
			static auto parsed = false;
			if (parsed)
			{
				return;
			}

			static std::string comand_line_buffer = GetCommandLineA();
			auto* command_line = comand_line_buffer.data();

			auto inq = false;

			std::vector<const char*> cmds;

			auto was_space = true;
			while (*command_line)
			{
				if (*command_line == '"')
				{
					inq = !inq;
				}

				if ((*command_line == '+' || *command_line == '-') && !inq && was_space)
				{
					const auto cmd = command_line + 1;
					if (*command_line == '+')
					{
						cmds.emplace_back(cmd);
					}

					*command_line = '\0';
				}

				was_space = *command_line == ' ';
				command_line++;
			}

			for (const auto& cmd : cmds)
			{
				command::execute(cmd, true);
			}

			parsed = true;
		}

		void execute_commands(const std::string& cmd);

		bool execute_alias(const std::string& name)
		{
			const auto iter = aliases.find(name);
			if (iter != aliases.end())
			{
				execute_commands(iter->second);
				return true;
			}

			return false;
		}

		void execute_args(const std::vector<std::string>& args)
		{
			const auto name = utils::string::to_lower(args[0]);
			const auto command = commands.find(name);

			if (command == commands.end())
			{
				if (!vars::var_command(args) && !execute_alias(name))
				{
					console::warn("Command \"%s\" not found\n", name.data());
				}

				return;
			}

			try
			{
				command->second(args);
			}
			catch (const std::exception& e)
			{
				console::error("Error executing command: %s\n", e.what());
			}
		}

		void execute_single(const std::string& cmd)
		{
			const auto args = tokenize_string(cmd);
			if (args.size() == 0)
			{
				return;
			}

			execute_args(args);
		}

		void queue_command(const std::string& cmd, const std::uint32_t wait = 0)
		{
			next_command_queue.access([&](command_queue_t& queue)
			{
				command_queue_entry_t entry{};
				entry.cmd = cmd;
				entry.wait = wait;
				queue.emplace_back(entry);
			});
		}

		std::vector<std::string> split_commands(const std::string& line)
		{
			std::vector<std::string> cmds;

			auto is_in_quotes = false;
			std::string current;
			for (auto i = 0; i < line.size(); i++)
			{
				if (line[i] == '"')
				{
					is_in_quotes = !is_in_quotes;
				}
				if (!is_in_quotes && line[i] == ';')
				{
					cmds.emplace_back(current);
					current.clear();
				}
				else
				{
					current.push_back(line[i]);
				}
			}

			if (!current.empty())
			{
				cmds.emplace_back(current);
			}

			return cmds;
		}

		bool get_next_command(const std::string& cmd, const std::size_t begin, std::size_t& end)
		{
			auto is_in_quotes = false;

			end = begin;
			for (end = begin; end < cmd.size(); end++)
			{
				if (cmd[end] == '"')
				{
					is_in_quotes = !is_in_quotes;
				}

				if (!is_in_quotes && cmd[end] == ';')
				{
					return true;
				}
			}

			return end != begin;
		}

		void execute_commands(const std::string& line)
		{
			std::size_t begin{};
			std::size_t end{};
			auto start1 = std::chrono::high_resolution_clock::now();
			while (get_next_command(line, begin, end))
			{
				const auto args = tokenize_string(line, begin, end);
				begin = end + 1;

				if (args.empty())
				{
					continue;
				}

				if (args[0] == "wait")
				{
					auto wait_time = 0;
					if (args.size() > 1)
					{
						wait_time = std::atoi(args[1].data());
					}

					auto next_commands = line.substr(begin);
					queue_command(next_commands, wait_time);
					return;
				}
				else
				{
					execute_args(args);
				}
			}
		}

		void exec_file(const std::string& data)
		{
			std::string cmd_buffer;

			auto is_in_comment = false;
			auto is_in_quotes = false;
			auto skip_line = false;

			for (auto i = 0ull; i < data.size(); i++)
			{
				if (data[i] == '\n' || data[i] == '\r')
				{
					if (!cmd_buffer.empty())
					{
						execute(cmd_buffer, true);
					}

					cmd_buffer.clear();
					is_in_quotes = false;
					is_in_comment = false;
					skip_line = false;
					continue;
				}

				if (skip_line)
				{
					continue;
				}

				const auto cur = data[i];
				const auto prev = i > 0 ? data[i - 1] : 0;
				const auto next = i < data.size() - 1 ? data[i + 1] : 0;

				if (!is_in_quotes && !is_in_comment && cur == '/' && next == '*')
				{
					is_in_comment = true;
					i++;
				}
				else if (is_in_comment && cur == '*' && next == '/')
				{
					is_in_comment = false;
					i++;
				}
				else if (!is_in_quotes && !is_in_comment && cur == '/' && next == '/')
				{
					skip_line = true;
					continue;
				}
				else if (!is_in_comment && cur == '"' && prev != '\\')
				{
					is_in_quotes = !is_in_quotes;
					cmd_buffer += cur;
				}
				else if (!is_in_comment)
				{
					cmd_buffer += cur;
				}
			}

			if (!cmd_buffer.empty())
			{
				execute(cmd_buffer, true);
				cmd_buffer.clear();
			}
		}

		bool exec_cfg(const std::string& name)
		{
			const auto file = utils::string::va("config\\%s", name.data());
			std::string data;
			if (!filesystem::read_file(file, &data))
			{
				return false;
			}

			exec_file(data);
			return true;
		}
	}

	params::params(const std::vector<std::string>& tokens)
		: tokens_(tokens)
	{
	}

	std::string params::get(const size_t index) const
	{
		if (this->tokens_.size() <= index)
		{
			return {};
		}

		return this->tokens_[index];
	}

	int params::get_int(const size_t index) const
	{
		if (this->tokens_.size() <= index)
		{
			return 0;
		}

		return std::atoi(this->tokens_[index].data());
	}

	float params::get_float(const size_t index) const
	{
		if (this->tokens_.size() <= index)
		{
			return 0;
		}

		return static_cast<float>(std::atof(this->tokens_[index].data()));
	}

	std::uint64_t params::get_uint64(const size_t index) const
	{
		if (this->tokens_.size() <= index)
		{
			return 0;
		}

		return std::strtoull(this->tokens_[index].data(), nullptr, 0);
	}

	std::string params::operator[](const size_t index) const
	{
		return this->get(index);
	}

	size_t params::size() const
	{
		return this->tokens_.size();
	}

	std::string params::join(const size_t index) const
	{
		std::string buffer;

		for (auto i = index; i < this->size(); i++)
		{
			buffer.append(this->tokens_[i]);

			if (i != this->size() - 1)
			{
				buffer.append(" ");
			}
		}

		return buffer;
	}

	void run_frame()
	{
		command_queue.access([](command_queue_t& queue)
		{
			next_command_queue.access([&](command_queue_t& next_queue)
			{
				queue.insert(queue.end(), next_queue.begin(), next_queue.end());
				next_queue.clear();
			});

			for (auto i = queue.begin(); i != queue.end(); )
			{
				if (i->wait == 0)
				{
					execute_commands(i->cmd);
					i = queue.erase(i);
					continue;
				}
				else
				{
					--i->wait;
				}

				++i;
			}
		});
	}

	void execute(const std::string& cmd, bool sync)
	{
		if (sync)
		{
			execute_commands(cmd);
		}
		else
		{
			queue_command(cmd);
		}
	}

	void add(const std::string& name, const callback& cb)
	{
		console::debug("Registering console command \"%s\"\n", name.data());
		commands.insert(std::make_pair(name, cb));
	}

	void add(const std::string& name, const callback_narg& cb)
	{
		add(name, [cb](const command::params&)
		{
			cb();
		});
	}

	void remove(const std::string& name)
	{
		commands.erase(name);
	}

	std::optional<std::string> find_command_name(const std::string& input)
	{
		if (input.size() < 2)
		{
			return {};
		}

		const auto lower = utils::string::to_lower(input);

		for (const auto& [name, command] : commands)
		{
			if (name.starts_with(lower))
			{
				return {name};
			}
		}

		return {};
	}

	std::vector<std::string> tokenize_string(const std::string& str, const std::size_t begin, const std::size_t end)
	{
		std::vector<std::string> res;
		std::string current_token;

		auto is_in_quotes = false;
		auto is_in_word = false;

		const auto add_token = [&](bool force = false)
		{
			if (!current_token.empty() || force)
			{
				if (current_token.starts_with("$"))
				{
					const auto var = vars::find(current_token.substr(1));
					if (var != nullptr)
					{
						current_token = var->current.to_string();
					}
				}

				res.emplace_back(current_token);
				current_token.clear();
			}
		};

		const auto end_cap = std::min(str.size(), end);

		auto was_space = false;
		for (auto i = begin; i < end_cap; i++)
		{
			const auto c = str[i];
			const auto prev = i > 0 ? str[i - 1] : 0;

			if (c == '"' && !is_in_word && prev != '\\')
			{
				if (is_in_quotes)
				{
					add_token(true);
				}

				was_space = false;
				is_in_quotes = !is_in_quotes;
				continue;
			}
			else if (c == ' ' || c == '\t')
			{
				if (is_in_quotes)
				{
					current_token.push_back(c);
				}
				else
				{
					if (was_space)
					{
						continue;
					}

					was_space = true;
					is_in_word = false;
					add_token();
					continue;
				}
			}
			else
			{
				if (!is_in_quotes)
				{
					is_in_word = true;
				}

				was_space = false;
				current_token.push_back(c);
			}
		}

		add_token();

		return res;
	}

	std::unordered_map<std::string, callback>& get_commands()
	{
		return commands;
	}

	std::unordered_map<std::string, std::string>& get_aliases()
	{
		return aliases;
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			command::add("exec", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto name = params.get(1);
				if (!exec_cfg(name))
				{
					console::warn("cfg file \"%s\" not found\n", name.data());
				}
			});

			command::add("alias", [](const params& params)
			{
				if (params.size() < 3)
				{
					for (const auto& alias : aliases)
					{
						console::info("alias \"%s\" \"%s\"\n", alias.first.data(), alias.second.data());
					}
				}
				else
				{
					const auto name = params.get(1);
					const auto cmd = params.get(2);
					aliases[name] = cmd;
					vars::write_config();
				}
			});
		}

		void post_load() override
		{
			if (!game::environment::is_dedi())
			{
				if (game::environment::is_mgo())
				{
					exec_cfg("config_mgo.cfg");
					exec_cfg("keys_mgo.cfg");
					exec_cfg("autoexec_mgo.cfg");

				}
				else
				{
					exec_cfg("config_tpp.cfg");
					exec_cfg("keys_tpp.cfg");
					exec_cfg("autoexec_tpp.cfg");
				}

				exec_cfg("autoexec.cfg");
			}

			parse_command_line();
		}

		void start() override
		{
			scheduler::loop(run_frame, scheduler::main);

			command::add("quit", game::tpp::ui::utility::UiUtilityImpl_::CallFoxQuit);

			command::add("startsound", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto id_s = params.get(1);
				const auto id = static_cast<unsigned int>(std::atoi(id_s.data()));

				const auto ui_inst = game::tpp::ui::menu::UiCommonDataManager_::GetInstance();
				if (ui_inst == nullptr)
				{
					return;
				}

				const auto sound_control = game::tpp::ui::menu::UiCommonDataManager_::GetSoundControl(ui_inst);
				if (sound_control == nullptr)
				{
					return;
				}

				game::tpp::ui::utility::StartSound(sound_control, id);
			});

			command::add("stopsound", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				const auto id_s = params.get(1);
				const auto id = static_cast<unsigned int>(std::atoi(id_s.data()));

				const auto ui_inst = game::tpp::ui::menu::UiCommonDataManager_::GetInstance();
				if (ui_inst == nullptr)
				{
					return;
				}

				const auto sound_control = game::tpp::ui::menu::UiCommonDataManager_::GetSoundControl(ui_inst);
				if (sound_control == nullptr)
				{
					return;
				}

				game::tpp::ui::utility::StopSound(sound_control, id);
			});

			// console hint
			command::add("wait", []()
			{

			});

			command::add("framestats", []()
			{
				const auto time_system = game::fox::GetTimeSystem();
				console::info("frametime: %f, fps: %i\n", time_system.frameTime, static_cast<int>(1.f / time_system.frameTime));
			});

			if (game::environment::is_tpp())
			{
				command::add("startmgo", []
				{
					utils::nt::start_process("mgsvmgo.exe");
					utils::nt::terminate();
				});
			}
			else
			{
				command::add("starttpp", []
				{
					utils::nt::start_process("mgsvtpp.exe");
					utils::nt::terminate();
				});
			}

			if (game::environment::is_mgo())
			{
				command::add("editavatar", []()
				{
					const auto script_vars = game::fox::GetQuarkSystemTable()->applicationSystem->mgo.scriptVars;
					if (script_vars->mgo.rulesetId != 4)
					{
						return;
					}

					scripting::script_exec("if (svars.seq_sequence == 3) then svars.seq_sequence = 4 end");
				});
			}
		}
	};
}

REGISTER_COMPONENT(command::component)
