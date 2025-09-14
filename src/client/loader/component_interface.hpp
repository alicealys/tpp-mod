#pragma once

class component_interface
{
public:
	virtual ~component_interface()
	{
	}

	virtual void pre_load()
	{
	}

	virtual void post_load()
	{
	}

	virtual void end()
	{
	}

	virtual void start()
	{
	}

	virtual void* load_import([[maybe_unused]] const std::string& library, [[maybe_unused]] const std::string& function)
	{
		return nullptr;
	}

	virtual bool is_supported()
	{
		return true;
	}
};
