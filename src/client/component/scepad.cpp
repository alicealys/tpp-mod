#include <std_include.hpp>
#include "scepad.hpp"
#include "loader/component_loader.hpp"
#include "scheduler.hpp"
#include "console.hpp"
#include "game/game.hpp"
#include <utils/hook.hpp>

namespace scepad
{
    namespace
    {
        struct TriggerEffect 
        {
            TriggerMode left_trigger_mode = TriggerMode::Normal;
            std::vector<int> left_parameters;
            TriggerMode right_trigger_mode = TriggerMode::Normal;
            std::vector<int> right_parameters;
        };

        static TriggerEffect generic_effect = { TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {5,8,2} };
        static bool gun_empty = false;
        static std::chrono::steady_clock::time_point last_time_fired = std::chrono::steady_clock::now();

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

            // Rifles
            am_mrs_4r = 27873,

            // Granade launchers
            rgl_220_stun = 27879,
            zorn_kp_sleep = 159074,

            // Sniper rifles
            am_mrs71_rifle = 93476,
            
            count
        };

        utils::hook::detour try_fire_hook;
        void* try_fire_stub(__int64 a1, __int64 a2, unsigned int a3) 
        {
            void* result = try_fire_hook.invoke<void*>(a1, a2, a3);

            if ((int*)result == 0) 
            {
                gun_empty = true;
            }
            else
            {
                gun_empty = false;
            }

            return result;
        }

        utils::hook::detour state_gun_fire_hook;
        void state_gun_fire_stub(__int64 a1, unsigned int a2, int a3)
        {
            last_time_fired = std::chrono::steady_clock::now();
            state_gun_fire_hook.invoke<void>(a1, a2, a3);
        }

        std::unordered_map<weapon, TriggerEffect> triggerPreset =
        {
            {weapon::am_mrs_4r, {TriggerMode::Normal, {0}, TriggerMode::AutomaticGun, {3,8,11}}},
            {weapon::am_mrs71_rifle, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {5,8,8}}},
            {weapon::rgl_220_stun, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {6,8,8}}},
            {weapon::zorn_kp_sleep, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {6,8,6}}},
        };

        bool is_player_firing() 
        {
            auto now = std::chrono::steady_clock::now();
            if ((now - last_time_fired) <= 100ms) 
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool is_player_action_blocked()
        {
            const auto inst = game::tpp::ui::hud::CommonDataManager_::GetInstance();
            const auto ui_inst = game::tpp::ui::menu::UiCommonDataManager_::GetInstance();

            if (inst == nullptr || ui_inst == nullptr)
            {
                return true;
            }

            if (game::tpp::ui::menu::UiCommonDataManager_::GetPauseMenuType(ui_inst) != 0 ||
                !game::tpp::ui::hud::CommonDataManager_::IsEndLoadingTips(inst) ||
                game::tpp::ui::menu::impl::MotherBaseDeviceSystemImpl_::IsDeviceOpend())
            {
                return true;
            }

            if (game::tpp::gm::player::player2System->player2System == nullptr ||
                game::tpp::gm::player::player2System->player2System->tpp.pad == nullptr)
            {
                return true;
            }

            if (game::tpp::gm::player::player2System->player2System->tpp.pad->mask != 0)
            {
                return true;
            }

            if (game::tpp::gm::player::impl::Player2UtilityImpl_::IsLoading())
            {
                return true;
            }

            return false;
        }

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
            if (!is_player_initialized())
                return -1;
            if (!game::environment::is_tpp())
                return -1;

            const auto player = game::tpp::gm::player::player2System->player2System;
            int weaponType = 0;
            player->tpp.controller->__vftable->GetCurrentWeapon(player->tpp.controller, &weaponType, player->tpp.localPlayerIndex);
            return weaponType;
        }

        int get_current_equip_slot() 
        {
            if (!is_player_initialized())
                return -1;
            if (!game::environment::is_tpp())
                return -1;

            const auto player = game::tpp::gm::player::player2System->player2System;
            int equipSlot = 0;
            player->tpp.controller->__vftable->GetCurrentEquipSlot(player->tpp.controller, player->tpp.localPlayerIndex);
            return equipSlot;
        }

        void update_scepad()
        {
            DSX::clearPayload();

            int weaponType = get_weapon_type();
            auto triggerIt = triggerPreset.find(static_cast<weapon>(weaponType));
            if (is_player_action_blocked()) 
            {
                DSX::setLeftTrigger(TriggerMode::Normal, {0});
                DSX::setRightTrigger(TriggerMode::Normal, { 0 });
                DSX::setRGB(255, 255, 255, 255);
            }
            else if (triggerIt != triggerPreset.end() && !gun_empty) 
            {
                TriggerEffect mode = triggerIt->second;
                if (mode.right_trigger_mode == TriggerMode::AutomaticGun && !is_player_firing()) 
                {
                    DSX::setLeftTrigger(generic_effect.left_trigger_mode, generic_effect.left_parameters);
                    DSX::setRightTrigger(generic_effect.right_trigger_mode, generic_effect.right_parameters);
                }
                else 
                {
                    DSX::setLeftTrigger(mode.left_trigger_mode, mode.left_parameters);
                    DSX::setRightTrigger(mode.right_trigger_mode, mode.right_parameters);
                }
            }
            else 
            {
                DSX::setLeftTrigger(generic_effect.left_trigger_mode, generic_effect.left_parameters);
                DSX::setRightTrigger(generic_effect.right_trigger_mode, generic_effect.right_parameters);
            }

            if (!is_player_action_blocked()) 
            {
                DSX::setRGB(0, 0, 100, 255);
            }

            if (DSX::sendPayload() != DSX::Success) {
                console::warn("[scepad] DSX++ client failed to send data!");
            }
        }

        void print_weapon_type() 
        {
            console::info("[scepad] held weapon: %d", get_weapon_type());
        }

        class component final : public component_interface
        {
        public:
            void pre_load() override
            {
                if (!game::environment::is_tpp()) return;
                state_gun_fire_hook.create(game::tpp::gm::player::impl::attack::AttackActionImpl_::StateGunFire.get(), state_gun_fire_stub);
                try_fire_hook.create(game::tpp::gm::player::impl::attack::AttackActionImpl_::TryFire.get(), try_fire_stub);   
            }

            void start() override
            {
                if (DSX::init() != DSX::Success) {
                    console::warn("[scepad] DSX++ client failed to initialize!");
                    return;
                }
                console::info("[scepad] DSX++ client initialized successfully!");
                
                scheduler::loop(update_scepad, scheduler::main, 30ms);
                scheduler::loop(print_weapon_type, scheduler::main, 2s);
            }
        };
    }
}

REGISTER_COMPONENT(scepad::component)