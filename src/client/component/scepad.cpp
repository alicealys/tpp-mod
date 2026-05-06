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
        enum class weapon
        {
            unknown_weapon = 0,

            // Pistols
            wu_spistol,
            wu_spistol_cb,
            wu_spistol_sb,
            wu_spistol_ap,
            wu_spistol_9,
            am_d114,
            am_d114_cb,
            am_d114_9,
            am_d114lb_45,
            am_d114lb_9,
            burkov,
            burkov_tb,
            burkov_hs,
            geist_p3,
            geist_p3_cb,
            wu_s333,
            wu_s324lb,

            // Granade launchers
            rgl_220_stun = 27879,
            zorn_kp_sleep = 159074,

            // Sniper rifles
            am_mrs71_rifle = 93476,
            count
        };

        std::unordered_map<weapon, ScePadTriggerEffectParam> triggerPreset = 
        {
            {weapon::am_mrs71_rifle, {SCE_PAD_TRIGGER_EFFECT_TRIGGER_MASK_R2, {0}, {
			   {SCE_PAD_TRIGGER_EFFECT_MODE_OFF, {0}, {}},
			   {SCE_PAD_TRIGGER_EFFECT_MODE_WEAPON, {0}, {.weaponParam = {6,8,8}}}
		    }}},

           {weapon::rgl_220_stun, {SCE_PAD_TRIGGER_EFFECT_TRIGGER_MASK_R2, {0}, {
			   {SCE_PAD_TRIGGER_EFFECT_MODE_OFF, {0}, {}},
			   {SCE_PAD_TRIGGER_EFFECT_MODE_VIBRATION, {0}, {.vibrationParam = {6,8,10}}}
            }}},
        };

        #define STEAM_INPUT_MAX_COUNT 16
        using InputHandle_t = uint64_t;
        InputHandle_t controllers[STEAM_INPUT_MAX_COUNT];

        using SteamAPI_SteamInput_t = void*(*)();
        SteamAPI_SteamInput_t fn_get_interface = nullptr;

        using SteamInput_SetDualSenseTriggerEffect_t = void(*)(void*, InputHandle_t, const void*);
        SteamInput_SetDualSenseTriggerEffect_t fn_set_trigger_effect = nullptr;

        using SteamInput_GetConnectedControllers_t = int(*)(void*, InputHandle_t*);
        SteamInput_GetConnectedControllers_t fn_get_controllers = nullptr;

        using SteamInput_Init_t = bool(*)(void*, bool);
        SteamInput_Init_t fn_init = nullptr;

        void* steam_input = nullptr;

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
            if (!game::environment::is_tpp()) return -1;

            const auto player = game::tpp::gm::player::player2System->player2System;
            int weaponType = 0;
            player->tpp.controller->__vftable->GetCurrentWeapon(player->tpp.controller, &weaponType, player->tpp.localPlayerIndex);
            return weaponType;
        }

        void update_scepad()
        {
            s_ScePadData data {};
            scePadReadState(padHandle, &data);

            int weaponType = get_weapon_type();
            auto triggerIt = triggerPreset.find(static_cast<weapon>(weaponType));
            if (triggerIt != triggerPreset.end())
            {
                fn_set_trigger_effect(steam_input, controllers[0], &triggerIt->second);
                //scePadSetTriggerEffect(padHandle, &triggerIt->second);
                console::info("[scepad] trigger effect applied: %d", weaponType);
            }
            
            console::info("[scepad] weapon id: %d", weaponType);
        }

        class component final : public component_interface
        {
        public:
            void pre_load() override 
            {

            }

            void start() override
            {
                //s_ScePadInitParam initParam {};
                //initParam.allowBT = true;

                //int init_res = scePadInit3(&initParam);
                //if(init_res == SCE_OK)
                //{
                    //console::info("[scepad] initialized successfully");
                    //padHandle = scePadOpen(1, 0, 0);
                    //console::info("[scepad] opened handle %d", padHandle);
                //}
                //else
                //{
                    //console::error("[scepad] failed to initialize duaLib");
                //}

                HMODULE steam = GetModuleHandleA("steam_api64.dll");
                if (!steam) return;

                fn_get_interface = (SteamAPI_SteamInput_t)GetProcAddress(steam, "SteamAPI_SteamInput_v006");
                fn_set_trigger_effect = (SteamInput_SetDualSenseTriggerEffect_t)GetProcAddress(steam, "SteamAPI_ISteamInput_SetDualSenseTriggerEffect");
                fn_get_controllers = (SteamInput_GetConnectedControllers_t)GetProcAddress(steam, "SteamAPI_ISteamInput_GetConnectedControllers");
                fn_init = (SteamInput_Init_t)GetProcAddress(steam, "SteamAPI_ISteamInput_Init");

                if (!fn_get_interface || !fn_set_trigger_effect || !fn_get_controllers || !fn_init)
                {
                    console::warn("[scepad] steam input functions couldn't be loaded!");
                    return;
                }

                steam_input = fn_get_interface();
                if(!steam_input)
                {
                    console::warn("[scepad] couldn't get steam input interface");
                    return;
                }

                console::info("[scepad] initialized");
                scheduler::loop(update_scepad, scheduler::pipeline::main, 10ms);
            }
        };
    }
}

REGISTER_COMPONENT(scepad::component)