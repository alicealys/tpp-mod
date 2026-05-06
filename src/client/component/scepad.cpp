#include <std_include.hpp>
#include "scepad.hpp"
#include "loader/component_loader.hpp"
#include "scheduler.hpp"
#include "console.hpp"

namespace scepad
{
    namespace
    {
        void update_scepad()
        {
            console::info("[scepad] test from update_scepad");
        }

        class component final : public component_interface
        {
        public:
            void pre_load() override
            {
                int init_res = scePadInit();
                if(init_res == SCE_OK){
                    console::info("[scepad] initialized successfully");
                }
            }

            void start() override
            {
                scheduler::loop(update_scepad, scheduler::pipeline::main, 1s);
            }
        };
    }
}

REGISTER_COMPONENT(scepad::component)