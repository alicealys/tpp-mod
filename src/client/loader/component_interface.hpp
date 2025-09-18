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

	virtual void post_start()
	{
	}

	virtual bool is_supported()
	{
		return true;
	}
};
