#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "console.hpp"
#include "matchmaking.hpp"
#include "command.hpp"
#include "scripting.hpp"
#include "mods.hpp"

#include <utils/hook.hpp>
#include <utils/concurrency.hpp>
#include <utils/io.hpp>

namespace custom_maps
{
    namespace
    {
        constexpr const auto map_cmd_id = 0xFF;
        constexpr const auto map_cmd_location_code = 0xFF;

        struct map_info_t
        {
            std::uint8_t id;
            std::uint8_t location_code;
            std::string location_pack;
        };

        using map_list_t = std::vector<map_info_t>;
        utils::concurrency::container<map_list_t> custom_map_list;
        std::string current_pack;
        game::fox::fs::MountPoint* usermap_handle = nullptr;

        std::optional<std::string> get_pack_base_name(const std::string& pack)
        {
            const auto start = pack.find_last_of('/');
            const auto end = pack.find_last_of('.');

            if (start < end && start != std::string::npos)
            {
                const auto name = pack.substr(start + 1, end - start - 1);
                return {name};
            }

            return {};
        }

        void remove_current_map_packfile()
        {
            if (usermap_handle != nullptr)
            {
                game::fox::fs::MountPoint_::Destroy(usermap_handle);
                usermap_handle = nullptr;
            }
        }

        void try_add_map_packfile(const std::string& name)
        {
            current_pack = name;

            const auto map_name = get_pack_base_name(name);
            if (!map_name.has_value())
            {
                return;
            }

            const auto usermap_path = std::format("usermaps/{}/{}.dat", map_name.value(), map_name.value());
            if (!utils::io::file_exists(usermap_path))
            {
                return;
            }

            console::debug("adding usermap packfile %s\n", usermap_path.data());
            usermap_handle = mods::add_packfile(usermap_path, "texture", 255);
        }

        game::fox::KernelString* get_location_package_path_stub(game::fox::KernelString* string, char* path)
        {
            try_add_map_packfile(path);
            return utils::hook::invoke<game::fox::KernelString*>(SELECT_VALUE_LANG(0x1400165F0, 0x0), string, path);
        }

        utils::hook::detour get_package_paths_hook;
        void get_package_paths_stub(game::fox::Array<game::fox::Path>* paths, unsigned short location_code)
        {
            console::debug("get package paths\n");
            remove_current_map_packfile();
            current_pack = {};

            const auto found = custom_map_list.access<bool>([&](map_list_t& list)
            {
                const auto iter = std::ranges::find_if(list.begin(), list.end(), [&](map_info_t& map)
                {
                    return map.location_code == location_code;
                });

                if (iter == list.end())
                {
                    return false;
                }

                game::fox::Path path{};
                try_add_map_packfile(iter->location_pack);
                path.id = game::fox::fs::PathCodeImpl_::FromString(iter->location_pack.data());
                game::fox::Array_::Path_PushBack(paths, &path);
                return true;
            });

            if (found)
            {
                return;
            }

            get_package_paths_hook.invoke<void>(paths, location_code);
        }

        std::string format_location_package(const std::string& name)
        {
            return std::format("/Assets/mgo/pack/location/{}/{}.fpk", name, name);
        }

        void register_map(const std::uint8_t id, const std::uint8_t location_code, const std::string& location_pack)
        {
            map_info_t map_info{};
            map_info.id = id;
            map_info.location_code = location_code;
            map_info.location_pack = location_pack;

            custom_map_list.access([&](map_list_t& list)
            {
                const auto iter = std::ranges::find_if(list.begin(), list.end(), [&](map_info_t& map)
                {
                    return map.id == id;
                });

                if (iter == list.end())
                {
                    list.emplace_back(map_info);
                }
                else
                {
                    *iter = map_info;
                }
            });
        }

        int location_code_to_map_id_stub(void* a1, unsigned char location_code)
        {
            const auto result = custom_map_list.access<int>([&](map_list_t& list)
            {
                const auto iter = std::ranges::find_if(list.begin(), list.end(), [&](map_info_t& map)
                {
                    return map.location_code == location_code;
                });

                if (iter != list.end())
                {
                    return static_cast<int>(iter->id);
                }

                return -1;
            });

            if (result != -1)
            {
                return result;
            }

            switch (location_code)
            {
            case 102:
                return 2;
            case 103:
                return 1;
            case 104:
                return 4;
            case 105:
                return 3;
            case 111:
                return 5;
            case 112:
                return 6;
            case 113:
                return 7;
            default:
                return 0;
            }
        }

        int map_id_to_location_code(int map_id)
        {
            const auto result = custom_map_list.access<int>([&](map_list_t& list)
            {
                const auto iter = std::ranges::find_if(list.begin(), list.end(), [&](map_info_t& map)
                {
                    return map.id == static_cast<std::uint8_t>(map_id);
                });

                if (iter != list.end())
                {
                    return static_cast<int>(iter->location_code);
                }

                return -1;
            });

            if (result != -1)
            {
                return result;
            }

            switch (map_id)
            {
            case 0:
                return 101;
            case 1:
                return 103;
            case 2:
                return 102;
            case 3:
                return 105;
            case 4:
                return 104;
            case 5:
                return 111;
            case 6:
                return 112;
            case 7:
                return 113;
            default:
                return 0;
            }
        }

        void map_id_to_location_code_stub(utils::hook::assembler& a)
        {
            a.shr(r8, 8);
            a.movsx(rax, r8b);

            a.push(rax);
            a.pushad64();
            a.mov(rcx, rax);
            a.call_aligned(map_id_to_location_code);
            a.mov(qword_ptr(rsp, 0x80), rax);
            a.popad64();
            a.pop(rax);

            a.mov(bl, al);

            a.jmp(SELECT_VALUE_LANG(0x1408A21AC, 0x0));
        }
    }

    std::string get_current_map_name()
    {
        if (current_pack.empty())
        {
            return {};
        }

        const auto name = get_pack_base_name(current_pack);
        if (name.has_value())
        {
            return name.value();
        }

        return {};
    }

    class component final : public component_interface
    {
    public:
        void pre_load() override
        {
            if (!game::environment::is_mgo())
            {
                return;
            }

            utils::hook::call(SELECT_VALUE_LANG(0x140743177, 0x0), get_location_package_path_stub);
            get_package_paths_hook.create(SELECT_VALUE_LANG(0x140743030, 0x0), get_package_paths_stub);

            utils::hook::jump(SELECT_VALUE_LANG(0x14089A7A0, 0x0), location_code_to_map_id_stub);
            utils::hook::jump(SELECT_VALUE_LANG(0x1408A2169, 0x0), utils::hook::assemble(map_id_to_location_code_stub), true);

            command::add("map", [](const command::params& params)
            {
                if (params.size() < 2)
                {
                    return;
                }

                const auto mapname = params.get(1);
                auto location_id = std::atoi(mapname.data());

                if (location_id == 0)
                {
                    const auto location_pack = format_location_package(mapname);
                    location_id = map_cmd_location_code;
                    register_map(map_cmd_id, map_cmd_location_code, location_pack);
                }

                auto ruleset_id = 2;
                if (params.size() >= 3)
                {
                    ruleset_id = params.get_int(2);
                }

                const auto script = std::format(
                    "vars.locationCode={};"
                    "vars.memberMin=0;"
                    "vars.missionCode=6;"
                    "vars.rulesetId={};"
                    "vars.briefingTime=0;"
                    "Mission.LoadLocation({{force=true}});"
                    "Mission.LoadMission({{force=true}});", location_id, ruleset_id);

                scripting::script_exec(script);
            });

            command::add("registermap", [](const command::params& params)
            {
                if (params.size() < 4)
                {
                    return;
                }

                const auto map_id = static_cast<std::uint8_t>(params.get_int(1));
                const auto location_code = static_cast<std::uint8_t>(params.get_int(2));
                const auto name = params.get(3);
                const auto location_pack = format_location_package(name);

                register_map(map_id, location_code, location_pack);
            });
        }
    };
}

REGISTER_COMPONENT(custom_maps::component)
