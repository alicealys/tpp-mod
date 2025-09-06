#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"
#include "console.hpp"
#include "patches.hpp"
#include "scheduler.hpp"

#include <utils/io.hpp>
#include <utils/string.hpp>

namespace command
{
	namespace
	{
		std::unordered_map<std::string, callback> commands;

		std::mutex queue_mutex;
		std::vector<std::string> command_queue;
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
			console::warn("Command \"%s\" not found\n", name.data());
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
		printf("]%s", cmd.data());
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

	class component final : public component_interface
	{
	public:
		void post_start() override
		{
			scheduler::loop(run_frame, scheduler::main);
		}
	};
}

REGISTER_COMPONENT(command::component)
