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

        bool is_player_initialized()
		{
			if (game::environment::is_tpp())
			{
				return game::tpp::gm::player::player2System->player2System != nullptr &&
					game::tpp::gm::player::player2System->player2System->tpp.controller != nullptr;
			}
			else
			{
				return game::tpp::gm::player::player2System->player2System != nullptr &&
					game::tpp::gm::player::player2System->player2System->mgo.controller != nullptr;
			}
		}

        int get_weapon_type()
        {
            if (!is_player_initialized()) return -1;
            if (!game::environment::is_tpp())
            {
                console::warn("[scepad] Not in TPP");
                return -1;
            }

            const auto player = game::tpp::gm::player::player2System->player2System;

            console::info("PlayerIdx: %d", player->tpp.localPlayerIndex);

            console::info("[scepad] calling GetCurrentEquipSlot()");
            uint8_t weaponSlot = player->tpp.controller->__vftable->GetCurrentEquipSlot(player->tpp.controller, player->tpp.localPlayerIndex);

            console::info("[scepad] GetCurrentEquipSlot returned: %d", weaponSlot);

            return 0;
        }

        void update_scepad()
        {
            s_SceLightBar lightbar {0,255,58};
            scePadSetLightBar(padHandle, &lightbar);
            s_ScePadData data {};
            scePadReadState(padHandle, &data);

            if(data.bitmask_buttons & SCE_BM_CROSS){
                console::info("[scepad] cross pressed");
            }

            get_weapon_type();
        }

        class component final : public component_interface
        {
        public:
            void pre_load() override {}

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