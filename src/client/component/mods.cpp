#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "mods.hpp"
#include "console.hpp"
#include "scheduler.hpp"

#include <utils/hook.hpp>
#include <utils/nt.hpp>
#include <utils/io.hpp>

namespace mods
{
    vars::var_ptr var_fs_mod_path;

    namespace
    {
        utils::hook::detour fs_module_init_hook;

        void add_patch_file(const std::string& path)
        {
            const auto handle = game::fox::fs::FileLocationManager_::CreatePackMountPoint("patch", path.data(), 0, 17);
            if (handle)
            {
                game::fox::fs::FileLocationManager_::SetIoHandleCount(handle, 0x10);
            }
        }

        void fs_module_init_stub()
        {
            console::info("[FS module] Init\n");

            fs_module_init_hook.invoke<void>();

            const auto& mod_path = var_fs_mod_path->latched.get_string();
            if (!mod_path.empty())
            {
                console::info("[Mods] Mod path: %s\n", mod_path.data());
                add_patch_file(std::format("{}\\mod.dat", mod_path));
            }
        }

        bool can_load_mod()
        {
            const auto inst = game::tpp::ui::hud::CommonDataManager_::GetInstance();
            const auto ui_inst = game::tpp::ui::menu::UiCommonDataManager_::GetInstance();
            if (inst == nullptr || ui_inst == nullptr)
            {
                return false;
            }

            if (game::tpp::ui::menu::UiCommonDataManager_::GetPauseMenuType(ui_inst) != 0 ||
                !game::tpp::ui::hud::CommonDataManager_::IsEndLoadingTips(inst))
            {
                return false;
            }

            return true;
        }
    }

    void load(const std::string& path, const std::optional<std::string>& arg)
    {
        auto cmd = std::format("+fs_mod_path {} {}", path, arg.value_or(""));
        utils::nt::relaunch_self(cmd, true);
        utils::nt::terminate();
    }

    void unload()
    {
        utils::nt::relaunch_self("", true);
        utils::nt::terminate();
    }

    class component final : public component_interface
    {
    public:
        void pre_load() override
        {
            var_fs_mod_path = vars::register_string("fs_mod_path", "", vars::var_flag_latched, "mod folder path");

            fs_module_init_hook.create(SELECT_VALUE(0x14003A960, 0x14003A830, 0x0, 0x0), fs_module_init_stub);

            command::add("unloadmod", []()
            {
                if (var_fs_mod_path->latched.get_string().empty())
                {
                    console::warn("no mod loaded\n");
                    return;
                }

                if (!can_load_mod())
                {
                    return;
                }

                unload();
            });

            command::add("loadmod", [](const command::params& params)
            {
                if (params.size() < 2)
                {
                    return;
                }

                const auto path = params.get(1);
                if (!utils::io::directory_exists(path))
                {
                    console::warn("mod path %s not found!\n", path.data());
                    return;
                }

                if (!can_load_mod())
                {
                    return;
                }

                load(path);
            });
        }
    };
}

REGISTER_COMPONENT(mods::component)
