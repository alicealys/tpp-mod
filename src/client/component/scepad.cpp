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
            if (!is_player_initialized()) return 0;

            const auto player = game::tpp::gm::player::player2System->player2System;
            void* weaponPtr = player->tpp.controller->__vftable->GetCurrentWeapon(player->tpp.controller, player->tpp.localPlayerIndex);

            console::info("[scepad] GetCurrentWeapon returned: %p", weaponPtr);

            if (!weaponPtr) {
                console::info("[scepad] Weapon pointer is null");
                return 0;
            }

            console::info("[scepad] Memory dump around weapon:");
            for (int i = 0; i < 8; ++i) {
                uint64_t* p = (uint64_t*)((char*)weaponPtr + i*32);
                console::info("  +0x%02X: %016llX %016llX %016llX %016llX", 
                            i*32, p[0], p[1], p[2], p[3]);
            }

            for (int i = 0; i < 16; ++i) {
                const char* str = (const char*)weaponPtr + i*8;
                if (strlen(str) > 3 && strlen(str) < 64) {
                    console::info("  Possible string @ +0x%02X: %s", i*8, str);
                }
            }

            return 0;
        }

        void update_scepad()
        {
            s_SceLightBar lightbar {0,255,0};
            scePadSetLightBar(padHandle, &lightbar);
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