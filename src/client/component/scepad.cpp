#include "scepad.hpp"
#include "loader/component_loader.hpp"
#include "scheduler.hpp"


namespace scepad
{
    namespace
    {
        void update_scepad()
        {

        }

        class component final : public component_interface
        {
        public:
            void pre_load() override
            {

            }

            void start() override
            {
                scheduler::loop(update_scepad, scheduler::pipeline::main, 1s);
            }
        };
    }
}

REGISTER_COMPONENT(scepad::component)