#include <std_include.hpp>
#include "component_loader.hpp"

void component_loader::register_component(std::unique_ptr<component_interface>&& component_)
{
	get_components().push_back(std::move(component_));
}

bool component_loader::pre_load()
{
	static auto handled = false;
	if (handled)
	{
		return true;
	}

	handled = true;

	try
	{
		for (const auto& component_ : get_components())
		{
			component_->pre_load();
		}
	}
	catch (premature_shutdown_trigger&)
	{
		return false;
	}

	return true;
}

bool component_loader::post_load()
{
	static auto handled = false;
	if (handled)
	{
		return true;
	}

	handled = true;

	clean();

	try
	{
		for (const auto& component_ : get_components())
		{
			component_->post_load();
		}
	}
	catch (premature_shutdown_trigger&)
	{
		return false;
	}

	return true;
}

void component_loader::start()
{
	static auto handled = false;
	if (handled)
	{
		return;
	}

	handled = true;

	for (const auto& component_ : get_components())
	{
		component_->start();
	}
}

void component_loader::post_start()
{
	static auto handled = false;
	if (handled)
	{
		return;
	}

	handled = true;

	for (const auto& component_ : get_components())
	{
		component_->post_start();
	}
}

void component_loader::end()
{
	static auto handled = false;
	if (handled) return;
	handled = true;

	for (const auto& component_ : get_components())
	{
		component_->end();
	}
}

void component_loader::clean()
{
	auto& components = get_components();
	for (auto i = components.begin(); i != components.end();)
	{
		if (!(*i)->is_supported())
		{
			(*i)->end();
			i = components.erase(i);
		}
		else
		{
			++i;
		}
	}
}

void component_loader::trigger_premature_shutdown()
{
	throw premature_shutdown_trigger();
}

std::vector<std::unique_ptr<component_interface>>& component_loader::get_components()
{
	using component_vector = std::vector<std::unique_ptr<component_interface>>;
	using component_vector_container = std::unique_ptr<component_vector, std::function<void(component_vector*)>>;

	static component_vector_container components(new component_vector, [](component_vector* component_vector)
	{
		end();
		delete component_vector;
	});

	return *components;
}
