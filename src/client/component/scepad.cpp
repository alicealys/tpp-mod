#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "scepad.hpp"
#include "scheduler.hpp"
#include "vars.hpp"
#include "binds.hpp"
#include "console.hpp"

#include <DSX++.h>
#include <utils/hook.hpp>
#include <utils/concurrency.hpp>

namespace scepad
{
    namespace
    {
        struct trigger_effect_t
        {
            TriggerMode left_trigger_mode = TriggerMode::Normal;
            std::vector<int> left_parameters;
            TriggerMode right_trigger_mode = TriggerMode::Normal;
            std::vector<int> right_parameters;
        };

        enum weapon_type_t
        {
            weapon_unknown = 0,
            weapon_tpp_submachine_gun = 159075,
            weapon_tpp_automatic_rifle = 27873,
            weapon_tpp_machine_gun = 93478,
            weapon_tpp_grenade_launcher = 27879,
            weapon_tpp_pistol = 159074,
            weapon_tpp_pistol2 = 165186,
            weapon_tpp_revolver = 165314,
            weapon_tpp_sniper_rifle = 93476,
            weapon_tpp_shotgun = 27877,
            // weapon_tpp_bionic_arm = 16654,
            // weapon_tpp_hand_of_jehuty = 16526,
            weapon_mgo_submachine_gun = 348867,
            weapon_mgo_automatic_rifle = 282305,
            weapon_mgo_automatic_rifle2 = 413377,
            weapon_mgo_machine_gun = 282822,
            weapon_mgo_machine_gun2 = 413894,
            weapon_mgo_grenade_launcher = 282311,
            weapon_mgo_pistol = 348866,
            weapon_mgo_sniper_rifle = 413892,
            weapon_mgo_shotgun = 282309,
        };

        vars::var_ptr var_dsx_enable;
        trigger_effect_t generic_effect = {TriggerMode::Normal, {0}, TriggerMode::Resistance, {7, 1}};

        struct state_t
        {
            bool action_blocked;
            int current_weapon;
            std::chrono::steady_clock::time_point last_time_fired;
        };

        utils::concurrency::container<state_t> state;

        utils::hook::detour state_gun_fire_hook;
        void state_gun_fire_stub(__int64 a1, unsigned int a2, int a3)
        {
            state.access([&](state_t& s)
            {
                s.last_time_fired = std::chrono::steady_clock::now();
            });
            state_gun_fire_hook.invoke<void>(a1, a2, a3);
        }

        std::unordered_map<weapon_type_t, trigger_effect_t> trigger_presets =
        {
            {weapon_tpp_automatic_rifle, {TriggerMode::Normal, {0}, TriggerMode::AutomaticGun, {3, 6, 11}}},
            {weapon_tpp_machine_gun, {TriggerMode::Normal, {0}, TriggerMode::AutomaticGun, {3, 8, 13}}},
            {weapon_tpp_submachine_gun, {TriggerMode::Normal, {0}, TriggerMode::AutomaticGun, {3, 8, 15}}},
            {weapon_tpp_sniper_rifle, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2, 3, 7}}},
            {weapon_tpp_grenade_launcher, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2, 3, 8}}},
            {weapon_tpp_pistol, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2, 3, 6}}},
            {weapon_tpp_pistol2, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2, 3, 6}}},
            {weapon_tpp_shotgun, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2, 3, 8}}},
            {weapon_tpp_revolver, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2, 3, 7}}},
            {weapon_mgo_automatic_rifle, {TriggerMode::Normal, {0}, TriggerMode::AutomaticGun, {3, 6, 11}}},
            {weapon_mgo_automatic_rifle2, {TriggerMode::Normal, {0}, TriggerMode::AutomaticGun, {3, 6, 11}}},
            {weapon_mgo_machine_gun, {TriggerMode::Normal, {0}, TriggerMode::AutomaticGun, {3, 8, 13}}},
            {weapon_mgo_machine_gun2, {TriggerMode::Normal, {0}, TriggerMode::AutomaticGun, {3, 8, 13}}},
            {weapon_mgo_submachine_gun, {TriggerMode::Normal, {0}, TriggerMode::AutomaticGun, {3, 8, 15}}},
            {weapon_mgo_sniper_rifle, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2, 3, 7}}},
            {weapon_mgo_grenade_launcher, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2, 3, 8}}},
            {weapon_mgo_pistol, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2, 3, 6}}},
            {weapon_mgo_shotgun, {TriggerMode::Normal, {0}, TriggerMode::SemiAutomaticGun, {2, 3, 8}}},
        };

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
            {
                return -1;
            }

            const auto player = game::tpp::gm::player::player2System->player2System;
            auto res = 0;
            if (game::environment::is_tpp())
            {
                player->tpp.controller->__vftable->GetCurrentWeapon(player->tpp.controller, &res, player->tpp.localPlayerIndex);
            }
            else
            {
                player->mgo.controller->__vftable->GetCurrentWeapon(player->mgo.controller, &res, player->mgo.localPlayerIndex);
            }

            return res;
        }

        void update_state()
        {
            state.access([&](state_t& s)
            {
                s.action_blocked = binds::is_player_action_blocked();
                s.current_weapon = get_weapon_id();
            });
        }

        void update_scepad()
        {
            const auto state_copy = state.access<state_t>([&](state_t& s)
            {
                return s;
            });

            DSX::clearPayload();

            const auto iter = trigger_presets.find(static_cast<weapon_type_t>(state_copy.current_weapon));
            if (state_copy.action_blocked)
            {
                DSX::setLeftTrigger(TriggerMode::Normal, {0});
                DSX::setRightTrigger(TriggerMode::Normal, {0});
                DSX::setRGB(255, 255, 255, 255);
            }
            else if (iter != trigger_presets.end())
            {
                const auto& mode = iter->second;
                if (mode.right_trigger_mode == TriggerMode::AutomaticGun && 
                    (std::chrono::steady_clock::now() - state_copy.last_time_fired) > 100ms)
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

            if (!state_copy.action_blocked)
            {
                DSX::setRGB(0, 0, 100, 255);
            }

            if (DSX::sendPayload() != DSX::Success)
            {
                console::error("[scepad] DSX++ client failed to send data!");
            }
        }

        class component final : public component_interface
        {
        public:
            void pre_load() override
            {
                var_dsx_enable = vars::register_bool("dsx_enable", true, 
                    vars::var_flag_latched | vars::var_flag_saved, "enable DSX integration");
            }

            void start() override
            {
                if (!var_dsx_enable->current.enabled())
                {
                    return;
                }

                if (DSX::init() != DSX::Success) 
                {
                    console::error("[scepad] DSX++ client failed to initialize");
                    return;
                }

                state_gun_fire_hook.create(SELECT_VALUE(0x14106FD20, 0x141060A60, 0x14106FD70, 0x141060130), state_gun_fire_stub);

                console::info("[scepad] DSX++ client initialized");

                scheduler::once([]
                {
                    scheduler::loop(update_scepad, scheduler::async, 30ms);
                }, scheduler::net);
                scheduler::loop(update_state, scheduler::main);
            }
        };
    }
}

REGISTER_COMPONENT(scepad::component)
