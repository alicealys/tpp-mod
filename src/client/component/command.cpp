#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "console.hpp"
#include "patches.hpp"
#include "scheduler.hpp"
#include "vars.hpp"

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
					command::execute(cmd);
					*command_line = '\0';
				}

				command_line++;
			}

			parsed = true;
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
			buffer.append(" ");
		}

		return buffer;
	}

	void execute_single(const std::string& cmd)
	{
		const auto args = utils::string::split(cmd, ' ');
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

	void run_frame()
	{
		parse_command_line();

		std::vector<std::string> queue_copy;

		{
			std::lock_guard _0(queue_mutex);
			queue_copy = command_queue;
			command_queue.clear();
		}

		for (const auto& cmd : queue_copy)
		{
			execute_single(cmd);
		}
	}

	void execute(const std::string& cmd)
	{
		std::lock_guard _0(queue_mutex);
		command_queue.emplace_back(cmd);
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

				const auto ui_inst = game::tpp::ui::menu::UiCommonDataManager::GetInstance();
				if (ui_inst == nullptr)
				{
					return;
				}

				const auto sound_control = game::tpp::ui::menu::UiCommonDataManager::GetSoundControl(ui_inst);
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

				const auto ui_inst = game::tpp::ui::menu::UiCommonDataManager::GetInstance();
				if (ui_inst == nullptr)
				{
					return;
				}

				const auto sound_control = game::tpp::ui::menu::UiCommonDataManager::GetSoundControl(ui_inst);
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
