#pragma once

#include "types/ui_element.hpp"
#include "../console.hpp"

namespace lui
{
	ui_element_ptr& get_root_element();

	void register_module(const std::function<void()>& module);

	template <typename F>
	class module_installer
	{
	public:
		module_installer(const char* name, F& initializer)
		{
			register_module([=]()
			{
				console::debug("LUI: initializing module \"%s\"\n", name);
				initializer();
			});
		}
	};
}

#define REGISTER_MODULE(__name__) \
namespace \
{ \
	static lui::module_installer module_(#__name__, __name__); \
} \
