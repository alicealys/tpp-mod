#pragma once

class component_interface
{
public:
	component_interface()
	{
	}

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

	virtual void game_initialized()
	{
	}

	virtual bool is_supported()
	{
		return true;
	}

#ifdef DEBUG
	void set_name(const std::string_view& name)
	{
		this->name_ = name;
	}

	std::string_view& get_name()
	{
		return this->name_;
	}

private:
	std::string_view name_;
#endif

};
