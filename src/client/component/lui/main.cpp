#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "main.hpp"
#include "input.hpp"
#include "flow_manager.hpp"
#include "scripting.hpp"

#include "../command.hpp"
#include "../scheduler.hpp"

namespace lui
{
	namespace
	{
		std::atomic_bool restart_requested = true;
		std::vector<std::function<void()>> modules;

		ui_element_ptr create_root_element()
		{
			auto root = ui_element::create();
			element_state_t state{};
			state.position.anchor = ANCHOR_ALL;
			root->register_animation_state("default", state);
			root->animate_to_state("default");
			return root;
		}

		void initialize()
		{
			flow_manager::initialize();

			get_root_element() = create_root_element();

			for (const auto& module : modules)
			{
				module();
			}

			scripting::start();
		}

		void draw_root()
		{
			draw_info_t draw_info{};
			draw_info.rect.left = 0.f;
			draw_info.rect.top = 0.f;
			draw_info.rect.right = 1280.f;
			draw_info.rect.bottom = 720.f;
			draw_info.alpha = 1.f;

			get_root_element()->draw(draw_info);
		}

		void update_ui()
		{
			if (restart_requested)
			{
				initialize();
				restart_requested = false;
			}

			flow_manager::update();
			input::update();
			draw_root();
			input::post_update();
			scripting::run_frame();
		}
	}

	ui_element_ptr& get_root_element()
	{
		static ui_element_ptr root;
		return root;
	}

	void register_module(const std::function<void()>& module)
	{
		modules.emplace_back(module);
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			command::add("lui_restart", []()
			{
				restart_requested = true;
			});

			scheduler::loop(update_ui, scheduler::main);
		}
	};
}

#ifdef DEBUG
REGISTER_COMPONENT(lui::component)
#endif
