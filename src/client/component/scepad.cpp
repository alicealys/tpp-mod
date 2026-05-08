#include <std_include.hpp>
#include "scepad.hpp"
#include "loader/component_loader.hpp"
#include "scheduler.hpp"
#include "console.hpp"
#include "game/game.hpp"
#include <utils/hook.hpp>
#include "vars.hpp"
#include "command.hpp"

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

        vars::var_ptr var_send_dsx_packets;
        static TriggerEffect generic_effect = { TriggerMode::Normal, {0}, TriggerMode::Resistance, {7,1} };

        static std::chrono::steady_clock::time_point last_time_fired = std::chrono::steady_clock::now();

        enum class weapon
        {
            unknown_weapon = 0,

            submachine_gun = 159075,
            automatic_rifle = 27873,
            machine_gun = 93478,
            granade_launcher = 27879,
            pistol = 159074,
            pistol2 = 165186,
            revolver = 165314,
            sniper_rifle = 93476,
            shotgun = 27877,
            //bionic_arm = 16654,
            //hand_of_jehuty = 16526,

            count
        };

        utils::hook::detour state_gun_fire_hook;
        void state_gun_fire_stub(__int64 a1, unsigned int a2, int a3)
        {
            last_time_fired = std::chrono::steady_clock::now();
            state_gun_fire_hook.invoke<void>(a1, a2, a3);
        }

        std::unordered_map<weapon, TriggerEffect> triggerPreset =
        {
            {weapon::automatic_rifle, {TriggerMode::Normal, {0}, TriggerMode::AutomaticGun, {3,6,11}}},
            {weapon::machine_gun, {TriggerMode::Normal, {0}, TriggerMode::AutomaticGun, {3,8,13}}},
            {weapon::submachine_gun, {TriggerMode::Normal, {0}, TriggerMode::AutomaticGun, {3,8,15}}},
            {weapon::sniper_rifle, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2,3,7}}},
            {weapon::granade_launcher, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2,3,8}}},
            {weapon::pistol, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2,3,6}}},
            {weapon::pistol2, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2,3,6}}},
            {weapon::shotgun, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2,3,8}}},
            {weapon::revolver, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2,3,7}}},
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

        int get_weapon_id()
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
            if (!var_send_dsx_packets->current.enabled())
            {
                return;
            }

            DSX::clearPayload();

            int weaponType = get_weapon_id();
            auto triggerIt = triggerPreset.find(static_cast<weapon>(weaponType));
            if (is_player_action_blocked())
            {
                DSX::setLeftTrigger(TriggerMode::Normal, { 0 });
                DSX::setRightTrigger(TriggerMode::Normal, { 0 });
                DSX::setRGB(255, 255, 255, 255);
            }
            else if (triggerIt != triggerPreset.end())
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

            if (!is_player_action_blocked())
            {
                DSX::setRGB(0, 0, 100, 255);
            }

            if (DSX::sendPayload() != DSX::Success) {
                console::warn("[scepad] DSX++ client failed to send data!");
            }
        }

        void print_weapon_id()
        {
            console::info("[scepad] held weapon: %d", get_weapon_id());
        }

        class component final : public component_interface
        {
        public:
            void pre_load() override
            {
                if (!game::environment::is_tpp()) return;
                state_gun_fire_hook.create(game::tpp::gm::player::impl::attack::AttackActionImpl_::StateGunFire.get(), state_gun_fire_stub);
                var_send_dsx_packets = vars::register_bool("send_dsx_packets", true, vars::var_flag_saved, "send sony controller gimmick data to DSX/DSY");
            }

            void start() override
            {
                if (!game::environment::is_tpp()) return;

                command::add("printweaponid", []
                {
                    scheduler::once(print_weapon_id, scheduler::main);
                });

                if (DSX::init() != DSX::Success) {
                    console::warn("[scepad] DSX++ client failed to initialize!");
                    return;
                }

                console::info("[scepad] DSX++ client initialized successfully!");
                scheduler::loop(update_scepad, scheduler::main, 30ms);
            }
        };
    }
}

REGISTER_COMPONENT(scepad::component)