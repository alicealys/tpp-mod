#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "mods.hpp"
#include "console.hpp"
#include "scheduler.hpp"
#include "filesystem.hpp"

#include <utils/hook.hpp>
#include <utils/nt.hpp>
#include <utils/io.hpp>

namespace mods
{
    vars::var_ptr var_fs_mod_path;

    namespace
    {
        utils::hook::detour fs_module_init_hook;

        struct mod_pack_file_t
        {
            std::string name;
            std::uint64_t type;
            std::uint32_t flags;
        };

        struct mod_info_t
        {
            std::string name;
            std::string author;
            std::string version;
            std::string path;
            std::vector<mod_pack_file_t> pack_files;
        };

        std::unordered_map<std::string, game::fox::fs::MountPoint*> custom_mount_points;

        mod_info_t current_mod;

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
                auto& flags_j = files[i]["flags"];

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

                if (flags_j.is_number_unsigned())
                {
                    file.flags = flags_j.get<std::uint32_t>();
                }

                info.pack_files.emplace_back(file);
            }
        }

        void add_patch_file(const std::string& path, const std::uint64_t type, const std::uint32_t flags)
        {
            const auto abs_path = std::filesystem::absolute(path);
            const auto path_str = abs_path.generic_string();

            if (custom_mount_points.contains(path_str))
            {
                return;
            }

            const auto handle = game::fox::fs::MountPoint_::CreateWithPackFile(path.data(), path_str.data(), type, flags);
            if (handle)
            {
                game::fox::fs::FileLocationManager_::SetIoHandleCount(handle, 0x10);
            }
            else
            {
                console::error("failed to create pack file %s\n", path.data());
            }

            custom_mount_points.insert(std::make_pair(path_str, handle));
        }

        void remove_patch_file(const std::string& path)
        {
            const auto abs_path = std::filesystem::absolute(path);
            const auto path_str = abs_path.generic_string();
            const auto iter = custom_mount_points.find(path_str);

            if (iter == custom_mount_points.end())
            {
                return;
            }

            console::debug("removing patch file %s\n", path_str.data());
            game::fox::fs::MountPoint_::Destroy(iter->second);
            custom_mount_points.erase(iter);
        }

        void set_mod(const std::string& path)
        {
            console::info("[Mods] Loading mod: %s\n", path.data());

            current_mod = {};
            current_mod.path = path;

            filesystem::register_path(path);

            const auto info_path = std::format("{}\\mod.json", path);

            std::string info_data;
            if (utils::io::read_file(info_path, &info_data))
            {
                parse_mod_info(info_data, current_mod);
            }

            for (const auto& file : current_mod.pack_files)
            {
                if (!file.name.ends_with(".dat"))
                {
                    continue;
                }

                const auto pack_path = std::format("{}\\{}", path, file.name);
                add_patch_file(pack_path, file.type, file.flags);
            }
        }

        void fs_module_init_stub()
        {
            console::info("[FS module] Init\n");

            fs_module_init_hook.invoke<void>();

            const auto& mod_path = var_fs_mod_path->latched.get_string();
            if (!mod_path.empty())
            {
                set_mod(mod_path);
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
    }

    void load(const std::string& path, const std::optional<std::string>& arg)
    {
        auto cmd = std::format("+fs_mod {} {}", path, arg.value_or(""));
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

            command::add("fs_addpatch", [](const command::params& params)
            {
                if (params.size() < 2)
                {
                    return;
                }

                const auto path = params.get(1);
                add_patch_file(path.data(), 0ull, 17);
            });

            command::add("fs_removepatch", [](const command::params& params)
            {
                if (params.size() < 2)
                {
                    return;
                }

                const auto path = params.get(1);
                remove_patch_file(path.data());
            });
        }
    };
}

REGISTER_COMPONENT(mods::component)
