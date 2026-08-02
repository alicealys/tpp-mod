#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "mods.hpp"
#include "console.hpp"
#include "scheduler.hpp"
#include "filesystem.hpp"
#include "scripting.hpp"

#include <utils/hook.hpp>
#include <utils/nt.hpp>
#include <utils/io.hpp>

namespace mods
{
    vars::var_ptr var_fs_mod_path;

    namespace
    {
        utils::hook::detour fs_module_init_hook;

        enum mod_flag_t
        {
            MOD_FLAG_NONE = (0 << 0),
            MOD_FLAG_NEEDS_RESTART = (1 << 0),
        };

        struct mod_pack_file_t
        {
            std::string name;
            std::uint64_t type;
            std::uint32_t flags;
            game::fox::fs::MountPoint* handle;
        };

        struct mod_info_t
        {
            std::uint32_t flags;
            std::string name;
            std::string author;
            std::string version;
            std::optional<std::string> path;
            std::vector<mod_pack_file_t> pack_files;
        };

        mod_info_t current_mod{};

        void parse_mod_info(const std::string& data, mod_info_t& info)
        {
            auto mod_info_j = nlohmann::json::parse(data, nullptr, false);
            if (mod_info_j.is_discarded())
            {
                return;
            }

            const auto try_string = [&](const std::string& key)
                -> std::string
            {
                auto& value = mod_info_j[key];
                if (value.is_string())
                {
                    return value.get<std::string>();
                }

                return {};
            };

            auto& flags_j = mod_info_j["flags"];
            if (flags_j.is_number_unsigned())
            {
                info.flags = flags_j.get<std::uint32_t>();
            }

            info.name = try_string("name");
            info.author = try_string("author");
            info.version = try_string("version");

            auto& files = mod_info_j["pack_files"];

            if (!files.is_array())
            {
                return;
            }

            for (auto i = 0u; i < files.size(); i++)
            {
                mod_pack_file_t file{};

                auto& name_j = files[i]["name"];
                auto& type_j = files[i]["type"];
                auto& file_flags_j = files[i]["flags"];

                if (!name_j.is_string())
                {
                    continue;
                }

                file.name = files[i]["name"].get<std::string>();

                if (type_j.is_string())
                {
                    const auto type = type_j.get<std::string>();
                    const auto type_hash = game::fox::FoxStrHash32(type.data(), type.size());
                    file.type = type_hash.id;
                }
                else if (type_j.is_number_unsigned())
                {
                    file.type = type_j.get<std::uint64_t>();
                }

                if (file_flags_j.is_number_unsigned())
                {
                    file.flags = file_flags_j.get<std::uint32_t>();
                }

                info.pack_files.emplace_back(file);
            }
        }

        void clear_mod(bool& needs_restart)
        {
            if (current_mod.path.has_value())
            {
                const auto& path = current_mod.path.value();
                console::info("[Mods] Unloading mod: %s\n", path.data());
                filesystem::unregister_path(path);
            }

            needs_restart |= (current_mod.flags & MOD_FLAG_NEEDS_RESTART) != 0;

            for (auto& file : current_mod.pack_files)
            {
                if (file.handle != nullptr)
                {
                    game::fox::fs::MountPoint_::Destroy(file.handle);
                }
            }

            current_mod = {};
        }

        bool set_mod(const std::string& path, bool& needs_restart)
        {
            const auto fs_path = std::filesystem::path(path);
            const auto normal_path = fs_path.lexically_normal().generic_string();

            if (!utils::io::directory_exists(normal_path) || !normal_path.starts_with("mods"))
            {
                console::error("[Mods] Invalid mod path %s\n", path.data());
                return false;
            }

            if (current_mod.path.has_value() && current_mod.path.value() == normal_path)
            {
                return false;
            }

            clear_mod(needs_restart);

            console::info("[Mods] Loading mod: %s\n", normal_path.data());

            current_mod = {};
            current_mod.path.emplace(normal_path);

            filesystem::register_path(normal_path);

            const auto info_path = std::format("{}\\mod.json", normal_path);

            std::string info_data;
            if (utils::io::read_file(info_path, &info_data))
            {
                parse_mod_info(info_data, current_mod);
            }

            needs_restart |= (current_mod.flags & MOD_FLAG_NEEDS_RESTART) != 0;

            for (auto& file : current_mod.pack_files)
            {
                if (!file.name.ends_with(".dat"))
                {
                    continue;
                }

                const auto pack_path = std::format("{}\\{}", normal_path, file.name);
                file.handle = add_packfile(pack_path, file.type, file.flags);
            }

            return true;
        }

        void fs_module_init_stub()
        {
            console::info("[FS module] Init\n");

            fs_module_init_hook.invoke<void>();

            const auto& mod_path = var_fs_mod_path->latched.get_string();
            if (!mod_path.empty())
            {
                auto needs_restart = false;
                set_mod(mod_path, needs_restart);
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

            const auto mission_code = game::tpp::ui::utility::GetCurrentLocationId();
            const auto location_code = game::tpp::ui::utility::GetCurrentMissionId();

            if (mission_code == 0xFFFF || mission_code == 1 || location_code == 0xFFFF || location_code == 1)
            {
                return false;
            }

            return true;
        }

        utils::hook::detour create_pack_mountpoint_hook;
        game::fox::fs::MountPoint* create_pack_mountpoint_stub(const char* type, const char* name, __int64 a3, unsigned int flags)
        {
            const auto res = create_pack_mountpoint_hook.invoke<game::fox::fs::MountPoint*>(type, name, a3, flags);
            if (res != nullptr)
            {
                console::debug("create mount point: %s %s %lli %i\n", type, name, a3, flags);
            }

            return res;
        }

        void reload_assets()
        {
            //const auto ui_system = game::environment::is_tpp()
            //    ? game::fox::GetQuarkSystemTable()->applicationSystem->tpp.uiSystem
            //    : game::fox::GetQuarkSystemTable()->applicationSystem->mgo.uiSystem;
            //
            //const auto menu_system = ui_system->menuSystem;
            //menu_system->__vftable->SetLangChange(menu_system, 1);
            
            scripting::script_exec("Mission.LoadLocation({force=true});Mission.LoadMission({force=true})");

#ifdef DEBUG
            command::execute("lui_restart");
#endif
        }
    }

    game::fox::fs::MountPoint* add_packfile(const std::string& path, const std::uint64_t type, const std::uint32_t flags)
    {
        const auto abs_path = std::filesystem::absolute(path);
        const auto path_str = abs_path.generic_string();

        const auto handle = game::fox::fs::MountPoint_::CreateWithPackFile(path.data(), path_str.data(), type, flags);
        if (handle)
        {
            game::fox::fs::FileLocationManager_::SetIoHandleCount(handle, 0x10);
        }
        else
        {
            console::error("failed to create pack file %s\n", path.data());
        }

        return handle;
    }

    game::fox::fs::MountPoint* add_packfile(const std::string& path, const std::string& type, const std::uint32_t flags)
    {
        const auto type_hash = game::fox::FoxStrHash32(type.data(), type.size());
        return add_packfile(path, type_hash.id, flags);
    }

    void load(const std::string& path, const std::optional<std::string>& arg)
    {
        auto needs_restart = false;
        if (!set_mod(path, needs_restart))
        {
            return;
        }

        vars::set_var(var_fs_mod_path, path, vars::var_source_internal);

        if (needs_restart)
        {
            auto cmd = std::format("+fs_mod \"{}\" {}", path, arg.value_or(""));
            utils::nt::relaunch_self(cmd, true);
            utils::nt::terminate();
            return;
        }

        reload_assets();
    }

    void unload()
    {
        auto needs_restart = false;

        clear_mod(needs_restart);
        vars::set_var(var_fs_mod_path, "", vars::var_source_internal);

        if (needs_restart)
        {
            utils::nt::relaunch_self("", true);
            utils::nt::terminate();
            return;
        }

        reload_assets();
    }

    class component final : public component_interface
    {
    public:
        void pre_load() override
        {
            var_fs_mod_path = vars::register_string("fs_mod", "", vars::var_flag_latched, "mod folder path");

            fs_module_init_hook.create(SELECT_VALUE(0x14003A960, 0x14003A830, 0x0, 0x0), fs_module_init_stub);
            create_pack_mountpoint_hook.create(game::fox::fs::MountPoint_::CreateWithPackFile, create_pack_mountpoint_stub);

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
