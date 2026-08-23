#include <std_include.hpp>

#include "component_loader.hpp"
#include "component/console.hpp"

void component_loader::register_component(std::unique_ptr<component_interface>&& component)
{
	get_components().push_back(std::move(component));
}

#ifdef DEBUG
#define REGISTER_PHASE(__name__) \
void component_loader::__name__() \
{ \
	static auto handled = false; \
	if (handled) \
	{ \
		return; \
	} \
	handled = true; \
	console::info("[component_loader] running " #__name__ "\n"); \
	for (const auto& component : get_components()) \
	{ \
		if (!component->is_supported()) \
		{ \
			continue; \
		} \
		const auto start = std::chrono::high_resolution_clock::now(); \
		component->__name__(); \
		const auto& name = component->get_name(); \
		const auto end = std::chrono::high_resolution_clock::now(); \
		const auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); \
		if (duration_ms > 0ull) \
		{ \
			console::debug("%s::%s took %llims\n", name.data(), #__name__, duration_ms); \
		} \
	} \
}
#else
#define REGISTER_PHASE(__name__) \
void component_loader::__name__() \
{ \
	static auto handled = false; \
	if (handled) \
	{ \
		return; \
	} \
	handled = true; \
	console::info("[component loader] running " #__name__ "\n"); \
	for (const auto& component : get_components()) \
	{ \
		if (!component->is_supported()) \
		{ \
			continue; \
		} \
		component->__name__(); \
	} \
}
#endif

REGISTER_PHASE(pre_load);
REGISTER_PHASE(post_load);
REGISTER_PHASE(start);
REGISTER_PHASE(post_start);
REGISTER_PHASE(game_initialized);
REGISTER_PHASE(end);

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
