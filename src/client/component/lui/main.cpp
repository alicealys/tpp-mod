#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "main.hpp"
#include "input.hpp"
#include "flow_manager.hpp"
#include "scripting.hpp"
#include "renderer.hpp"

#include "../command.hpp"
#include "../scheduler.hpp"

namespace lui
{
	namespace
	{
		std::atomic_bool restart_requested = true;
		std::vector<std::function<void()>> modules;
		std::vector<std::weak_ptr<ui_element>> element_list;

		ui_element_ptr create_root_element()
		{
			auto root = ui_element::create();
			root->set_id("uiroot");
			element_state_t state{};
			state.position.anchor = ANCHOR_ALL;
			root->register_animation_state("default", state);
			root->animate_to_state("default");
			return root;
		}

		void cleanup_elements()
		{
			for (auto i = element_list.begin(); i != element_list.end(); )
			{
				if (i->expired())
				{
					i = element_list.erase(i);
					continue;
				}

				auto element = i->lock();
				if (element->get_parent() == nullptr)
				{
					element->close(true);
					i = element_list.erase(i);
					continue;
				}

				++i;
			}
		}

		void initialize()
		{
			auto& root_element = get_root_element();
			if (root_element != nullptr)
			{
				root_element->close();
			}

			for (auto& element : element_list)
			{
				if (!element.expired())
				{
					element.lock()->close(false);
				}
			}

			element_list.clear();
			flow_manager::initialize();

			root_element = create_root_element();

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

			cleanup_elements();
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

	void track_element(const std::weak_ptr<ui_element>& element)
	{
		element_list.emplace_back(element);
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			//material_create_hook.create(game::fox::gr::dg::ShaderTechniqueManager_::GetResourceHandle.get(), material_create_stub);
			//material_create_hook2.create(0x140277230, material_create_stub2);
			//material_create_hook3.create(0x143C34EE0, material_create_stub3);
			command::add("lui_restart", []()
			{
				restart_requested = true;
			});

			scheduler::loop(update_ui, scheduler::main);
			flow_manager::load();
			renderer::load();
		}

		void end() override
		{
			renderer::end();
		}
	};
}

//#ifdef DEBUG
REGISTER_COMPONENT(lui::component)
//#endif
