#include <std_include.hpp>
#include "scepad.hpp"
#include "loader/component_loader.hpp"
#include "scheduler.hpp"
#include "console.hpp"
#include "game/game.hpp"

namespace scepad
{
    namespace
    {
        static int padHandle = 0;

        int get_weapon_type()
        {
            const auto player = game::tpp::gm::player::player2System->player2System;
            int currentWeapon = player->tpp.controller->__vftable->GetCurrentWeapon(player->tpp.controller, player->tpp.localPlayerIndex);
            return currentWeapon;
        }

        void update_scepad()
        {
            s_SceLightBar lightbar {0,255,0};
            scePadSetLightBar(padHandle, &lightbar);
            console::info("[scepad] Current weapon: %d", get_weapon_type());
        }

        class component final : public component_interface
        {
        public:
            void pre_load() override
            {

            }

            void start() override
            {
                s_ScePadInitParam initParam {};
                initParam.allowBT = true;

                int init_res = scePadInit3(&initParam);
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