#pragma once
#include "component_interface.hpp"

class component_loader final
{
public:
	template <typename T>
	class installer final
	{
		static_assert(std::is_base_of<component_interface, T>::value, "component has invalid base class");

	public:
#ifdef DEBUG
		installer(const std::string_view& name)
		{
			auto inst = std::make_unique<T>();
			inst->set_name(name);
			register_component(std::move(inst));
		}
#else
		installer()
		{
			register_component(std::make_unique<T>());
		}
#endif
	};

	static void register_component(std::unique_ptr<component_interface>&& component);

	static void pre_load();
	static void post_load();

	static void start();
	static void post_start();

	static void game_initialized();

	static void end();

private:
	static std::vector<std::unique_ptr<component_interface>>& get_components();
};

#ifdef DEBUG
#define REGISTER_COMPONENT(name) \
namespace \
{ \
	static component_loader::installer<name> component{#name}; \
}
#else
#define REGISTER_COMPONENT(name) \
namespace \
{ \
	static component_loader::installer<name> component{}; \
}
#endif
