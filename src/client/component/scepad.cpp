#include <std_include.hpp>
#include "scepad.hpp"
#include "loader/component_loader.hpp"
#include "scheduler.hpp"
#include "console.hpp"

namespace scepad
{
    namespace
    {
        static int padHandle = 0;

        void update_scepad()
        {
            console::info("[scepad] test from update_scepad");
        }

        class component final : public component_interface
        {
        public:
            void pre_load() override
            {

            }

            void start() override
            {
                int init_res = scePadInit();
                if(init_res == SCE_OK)
                {
                    console::info("[scepad] initialized successfully");
                    padHandle = scePadOpen(1, 0, 0);
                    console::info("[scepad] opened handle %d", padHandle);
                }
                else
                {
                    console::error("[scepad] failed to initialize duaLib");
                }

                scheduler::loop(update_scepad, scheduler::pipeline::main, 1s);
            }
        };
    }
}

REGISTER_COMPONENT(scepad::component)