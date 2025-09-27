#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "console.hpp"
#include "patches.hpp"
#include "scheduler.hpp"
#include "vars.hpp"
#include "filesystem.hpp"

#include <utils/io.hpp>
#include <utils/string.hpp>

namespace command
{
	namespace
	{
		std::unordered_map<std::string, callback> commands;

		std::mutex queue_mutex;
		std::vector<std::string> command_queue;

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

			while (*command_line)
			{
				if (*command_line == '"')
				{
					inq = !inq;
				}
				// look for a + separating character
				// if commandLine came from a file, we might have real line seperators
				if ((*command_line == '+' && !inq) || *command_line == '\n' || *command_line == '\r')
				{
					const auto cmd = command_line + 1;
					command::execute(cmd, true);
					*command_line = '\0';
				}

				command_line++;
			}

			parsed = true;
		}

		std::vector<std::string> tokenize_string(const std::string& str)
		{
			std::vector<std::string> res;

			const auto args = utils::string::split(str, ' ');
			for (const auto& arg : args)
			{
				if (!arg.empty())
				{
					res.emplace_back(arg);
				}
			}

			return res;
		}

		void execute_single(const std::string& cmd)
		{
			const auto args = tokenize_string(cmd);
			if (args.size() == 0)
			{
				return;
			}

			const auto name = utils::string::to_lower(args[0]);
			const auto command = commands.find(name);
			if (command == commands.end())
			{
				if (!vars::var_command(args))
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

		void execute_commands(const std::string& line)
		{
			const auto cmds = utils::string::split(line, ';');
			for (const auto& cmd : cmds)
			{
				execute_single(cmd);
			}
		}

		void exec_file(const std::string& data)
		{
			auto is_in_comment = false;
			const auto lines = utils::string::split_lines(data);

			for (const auto& line : lines)
			{
				std::string cmd_buffer;

				auto is_in_quotes = false;

				for (auto i = 0ull; i < line.size(); i++)
				{
					const auto cur = line[i];
					const auto next = i < line.size() - 1 ? line[i + 1] : 0;

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
						break;
					}
					else if (!is_in_comment && cur == '"')
					{
						is_in_quotes = !is_in_quotes;
					}
					else if (!is_in_comment)
					{
						cmd_buffer += cur;
					}
				}

				if (!cmd_buffer.empty())
				{
					execute(cmd_buffer, true);
				}
			}
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
		std::vector<std::string> queue_copy;

		{
			std::lock_guard _0(queue_mutex);
			queue_copy = command_queue;
			command_queue.clear();
		}

		for (const auto& cmd : queue_copy)
		{
			execute_commands(cmd);
		}
	}

	void execute(const std::string& cmd, bool sync)
	{
		if (sync)
		{
			execute_commands(cmd);
		}
		else
		{
			std::lock_guard _0(queue_mutex);
			command_queue.emplace_back(cmd);
		}
	}

	void add(const std::string& name, const callback& cb)
	{
		console::info("Registering console command \"%s\"\n", name.data());
		commands.insert(std::make_pair(name, cb));
	}

	void add(const std::string& name, const callback_narg& cb)
	{
		add(name, [cb](const command::params&)
		{
			cb();
		});
	}

	std::optional<std::string> find_command_name(const std::string& input)
	{
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

				const auto file = params.get(1);
				std::string data;
				if (!filesystem::read_file(file, &data))
				{
					console::warn("cfg file \"%s\" not found\n", file.data());
					return;
				}

				exec_file(data);
			});
		}

		void post_load() override
		{
			if (!game::environment::is_dedi())
			{
				command::execute("exec config.cfg", true);
			}

			parse_command_line();
		}

		void start() override
		{
			scheduler::loop(run_frame, scheduler::main);

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
		}
	};
}

REGISTER_COMPONENT(command::component)
