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
                {weapon::am_mrs71_rifle, {SCE_PAD_TRIGGER_EFFECT_TRIGGER_MASK_R2, {0}, {{SCE_PAD_TRIGGER_EFFECT_MODE_OFF, {0}, {}}, {SCE_PAD_TRIGGER_EFFECT_MODE_WEAPON, {0}, {.weaponParam = {6, 8, 8}}}}}},

                {weapon::rgl_220_stun, {SCE_PAD_TRIGGER_EFFECT_TRIGGER_MASK_R2, {0}, {{SCE_PAD_TRIGGER_EFFECT_MODE_OFF, {0}, {}}, {SCE_PAD_TRIGGER_EFFECT_MODE_VIBRATION, {0}, {.vibrationParam = {6, 8, 10}}}}}},
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

            }
        };
    }
}

REGISTER_COMPONENT(scepad::component)