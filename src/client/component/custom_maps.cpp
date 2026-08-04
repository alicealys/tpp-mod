#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "console.hpp"
#include "matchmaking.hpp"
#include "command.hpp"
#include "scripting.hpp"
#include "custom_maps.hpp"
#include "scheduler.hpp"

#include "utils/steam.hpp"

#include <utils/hook.hpp>
#include <utils/concurrency.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>
#include <utils/cryptography.hpp>

namespace custom_maps
{
    namespace
    {
        constexpr const auto map_cmd_id = 0xFF;
        constexpr const auto map_cmd_location_code = 0xFF;
        constexpr const auto usermap_location_code_begin = 150;

        union usermap_id_t
        {
            struct
            {
                std::uint8_t id;
                std::uint8_t location_code;
                std::uint16_t reserved;
            } fields;
            std::uint32_t packed;
        };

        struct map_info_t
        {
            bool is_builtin;
            std::uint8_t location_code;
            std::string name;
            std::string location_pack;
            std::uint32_t hash;
        };

        std::string current_pack;
        game::fox::fs::MountPoint* usermap_handle = nullptr;

        std::mutex map_list_mutex;
        std::array<map_info_t, 0x100> map_list;

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

        std::string format_usermap_path(const std::string& name)
        {
            return std::format("usermaps/{}/{}.dat", name, name);
        }

        std::string format_location_package(const std::string& name)
        {
            return std::format("/Assets/mgo/pack/location/{}/{}.fpk", name, name);
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

            const auto usermap_path = format_usermap_path(map_name.value());
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

        map_info_t* get_map_by_location(const std::uint8_t location_code)
        {
            if (location_code == map_cmd_location_code)
            {
                return &map_list[map_cmd_id];
            }

            for (auto i = 0; i < map_list.size(); i++)
            {
                if (map_list[i].location_code == 0)
                {
                    break;
                }

                if (map_list[i].location_code == location_code)
                {
                    return &map_list[i];
                }
            }

            return nullptr;
        }

        utils::hook::detour get_package_paths_hook;
        void get_package_paths_stub(game::fox::Array<game::fox::Path>* paths, unsigned short location_code)
        {
            std::lock_guard _0(map_list_mutex);

            remove_current_map_packfile();
            current_pack = {};

            const auto map_info = get_map_by_location(static_cast<std::uint8_t>(location_code));
            console::debug("get package paths %i: %p\n", location_code, map_info);
            if (map_info == nullptr)
            {
                get_package_paths_hook.invoke<void>(paths, location_code);
                return;
            }

            game::fox::Path path{};
            try_add_map_packfile(map_info->location_pack);
            path.id = game::fox::fs::PathCodeImpl_::FromString(map_info->location_pack.data());
            game::fox::Array_::Path_PushBack(paths, &path);
        }

        std::uint8_t get_builtin_map_count()
        {
            auto count = 0;
            for (auto i = 0; i < map_list.size(); i++)
            {
                if (map_list[i].is_builtin)
                {
                    ++count;
                }
                else
                {
                    break;
                }
            }

            return static_cast<std::uint8_t>(count);
        }

        void register_builtin_map(const std::uint8_t index, const std::uint8_t location_code, const std::string& name, const std::string& location_pack)
        {
            map_info_t map_info{};
            map_info.is_builtin = true;
            map_info.location_code = location_code;
            map_info.name = name;
            map_info.location_pack = location_pack;
            map_list[index] = map_info;

            console::debug("register builtin map: %i %i %s %s\n", index, location_code, name.data(), location_pack.data());
        }

        bool register_custom_map(const std::uint8_t index, const std::uint8_t location_code, const std::string& name, const std::uint32_t hash)
        {
            if (index < get_builtin_map_count() || index >= map_list.size() - 1)
            {
                return false;
            }

            console::debug("register custom map: %i %i %s %X\n", index, location_code, name.data(), hash);

            map_info_t map_info{};
            map_info.location_code = location_code;
            map_info.name = name;
            map_info.location_pack = format_location_package(name);
            map_info.hash = hash;
            map_list[index] = map_info;
            return true;
        }

        int location_code_to_map_id_stub(void* a1, unsigned char location_code)
        {
            for (auto i = 0; i < map_list.size(); i++)
            {
                if (map_list[i].location_code == location_code)
                {
                    return i;
                }
            }

            return 0;
        }

        int map_id_to_location_code(int map_id)
        {
            if (map_id < 0 || map_id > 255)
            {
                return 0;
            }

            return map_list[map_id].location_code;
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

            a.jmp(SELECT_VALUE_LANG(0x1408A218C, 0x0));
        }

        void on_leave_lobby(game::mgo_match_t* match, game::steam_id lobby_id)
        {
            std::lock_guard _0(map_list_mutex);
            register_usermaps();
        }

        void on_match_start(game::mgo_match_t* match, game::steam_id lobby_id)
        {
            if (match->lobby_owner.bits == matchmaking::get_current_steam_id().bits)
            {
                const auto map_id = match->match_rules.slots[match->match_rules.pl_current_match].m_map_id;
                if (map_id >= 0 && map_id < map_list.size())
                {
                    utils::steam::set_lobby_data(lobby_id, "map_name", map_list[map_id].name);
                }
                else
                {
                    utils::steam::set_lobby_data(lobby_id, "map_name", "");
                }
            }
        }

        void on_start_transition(game::mgo_match_t* match, game::steam_id lobby_id)
        {
            if (match->lobby_owner.bits != matchmaking::get_current_steam_id().bits)
            {
                if (mods::try_download_mod(match, lobby_id))
                {
                    command::execute("disconnect");
                }
            }
        }
    }

    void on_lobby_create(game::mgo_match_t* match, game::steam_id lobby_id)
    {
        std::lock_guard _0(map_list_mutex);

        register_usermaps();

        auto index = 0;
        for (auto i = 0; i < map_list.size(); i++)
        {
            if (map_list[i].is_builtin)
            {
                continue;
            }

            if (map_list[i].location_code == 0)
            {
                break;
            }

            auto idx = index++;
            usermap_id_t id{};
            id.fields.id = static_cast<std::uint8_t>(i);
            id.fields.location_code = map_list[i].location_code;

            utils::steam::set_lobby_data(lobby_id, "usermap_id", id.packed, idx);
            utils::steam::set_lobby_data(lobby_id, "usermap_name", map_list[i].name, idx);

            const auto map_file = std::format("usermaps\\{}\\{}.dat", map_list[i].name, map_list[i].name);

            mods::add_hash_job(map_file, [lobby_id, idx](const std::uint32_t hash)
            {
                utils::steam::set_lobby_data(lobby_id, "usermap_hash", hash, idx);
            });
        }

        utils::steam::set_lobby_data(lobby_id, "usermap_num", index);
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

    bool is_builtin_location_id(const std::int32_t map_id)
    {
        switch (map_id)
        {
        case 101:
        case 102:
        case 103:
        case 104:
        case 105:
        case 111:
        case 112:
        case 113:
        case 114: //
        case 115: //
        case 116: //
            return true;
        }

        return false;
    }

    void sync_registered_maps(const game::steam_id lobby_id)
    {
        std::lock_guard _0(map_list_mutex);

        auto map_count = utils::steam::get_lobby_data<std::int32_t>(lobby_id, "usermap_num");
        map_count = std::clamp(map_count, 0, 32);

        for (auto i = 0; i < map_count; i++)
        {
            usermap_id_t map_id{};
            map_id.packed = utils::steam::get_lobby_data<std::uint32_t>(lobby_id, "usermap_id", i);
            const auto name = utils::steam::get_lobby_data<std::string>(lobby_id, "usermap_name", i, 32);
            const auto hash = utils::steam::get_lobby_data<std::uint32_t>(lobby_id, "usermap_hash", i);

            if (!mods::validate_file_name(name) || hash == 0u)
            {
                continue;
            }

            register_custom_map(map_id.fields.id, map_id.fields.location_code, name, hash);
        }
    }

    bool check_download_map(const game::steam_id lobby_id, std::string& error, 
        const std::string& base_url, std::vector<mods::mod_download_file_t>& files)
    {
        const auto map_id = utils::steam::get_lobby_data<std::uint32_t>(lobby_id, "map_id");
        if (map_id < get_builtin_map_count())
        {
            return false;
        }

        if (map_id >= map_list.size())
        {
            error = "Server is running an invalid map";
            return false;
        }

        const auto& map_info = map_list[map_id];
        if (map_info.location_code == 0)
        {
            error = "Server is running an unregistered custom map";
            return false;
        }

        if (is_builtin_location_id(map_info.location_code))
        {
            return false;
        }

        if (base_url.empty())
        {
            error = "Server is running a custom map, but has no URL to download it from!";
            return false;
        }

        const auto map_path = format_usermap_path(map_info.name);
        const auto map_hash_path = map_path + ".hash";

        mods::mod_download_file_t file{};
        file.path = std::format("usermaps/{}/{}.dat", map_info.name, map_info.name);
        file.hash = map_info.hash;
        file.url = base_url + file.path;

        if (utils::io::file_exists(map_path) && utils::io::file_exists(map_hash_path))
        {
            const auto current_map_hash_txt = utils::io::read_file(map_hash_path);
            const auto current_map_hash = std::strtoul(current_map_hash_txt.data(), nullptr, 0);

            if (file.hash == current_map_hash)
            {
                return false;
            }
        }

        files.emplace_back(file);
        return true;
    }

    void register_builtin_maps()
    {
        register_builtin_map(0, 101, "afc0", "/Assets/mgo/pack/location/afc0/afc0.fpk");
        register_builtin_map(1, 103, "afda", "/Assets/mgo/pack/location/afda/afda.fpk");
        register_builtin_map(2, 102, "afn0", "/Assets/mgo/pack/location/afn0/afn0.fpk");
        register_builtin_map(3, 105, "cuba", "/Assets/mgo/pack/location/cuba/cuba.fpk");
        register_builtin_map(4, 104, "afc1", "/Assets/mgo/pack/location/afc1/afc1.fpk");
        register_builtin_map(5, 111, "mba0", "/Assets/mgo/pack/location/mba0/mba0.fpk");
        register_builtin_map(6, 112, "sva0", "/Assets/mgo/pack/location/sva0/sva0.fpk");
        register_builtin_map(7, 113, "rma0", "/Assets/mgo/pack/location/rma0/rma0.fpk");
        register_builtin_map(8, 114, "mba1", "/Assets/mgo/pack/location/mba1/mba1.fpk");
    }

    void register_usermaps()
    {
        const auto maps = utils::io::list_files("usermaps");

        auto cur_index = get_builtin_map_count();
        auto cur_location_code = usermap_location_code_begin;

        for (auto i = 0; i < map_list.size(); i++)
        {
            if (!map_list[i].is_builtin)
            {
                map_list[i] = {};
            }
        }

        for (auto& map : maps)
        {
            if (!utils::io::directory_exists(map))
            {
                continue;
            }

            const auto name_pos = map.find_last_of("\\/");
            const auto name = map.substr(name_pos + 1);
            const auto map_file = std::format("{}\\{}.dat", map, name);

            if (!utils::io::file_exists(map_file))
            {
                continue;
            }

            const auto index = cur_index++;
            const auto location_code = cur_location_code++;

            if (index >= 255 || cur_location_code >= 255)
            {
                break;
            }

            register_custom_map(static_cast<std::uint8_t>(index), static_cast<std::uint8_t>(location_code), name, {});
        }
    }

    void register_maps()
    {
        register_builtin_maps();
        register_usermaps();
    }

    std::uint8_t get_map_id(const std::string& name)
    {
        std::lock_guard _0(map_list_mutex);
        for (auto i = 0; i < map_list.size(); i++)
        {
            if (map_list[i].name == name)
            {
                return static_cast<std::uint8_t>(i);
            }
        }

        return 0u;
    }

    std::string get_map_name(const std::uint8_t index)
    {
        return map_list[index].name.data();
    }

    std::uint8_t get_map_count()
    {
        auto count = 0;
        for (auto i = 0; i < map_list.size(); i++)
        {
            if (map_list[i].location_code != 0)
            {
                ++count;
            }
        }

        return static_cast<std::uint8_t>(count);
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

            utils::hook::call(SELECT_VALUE_LANG(0x140743197, 0x0), get_location_package_path_stub);
            get_package_paths_hook.create(SELECT_VALUE_LANG(0x140742DF0, 0x0), get_package_paths_stub);

            utils::hook::jump(SELECT_VALUE_LANG(0x14089A780, 0x0), location_code_to_map_id_stub);
            utils::hook::jump(SELECT_VALUE_LANG(0x1408A2149, 0x0), utils::hook::assemble(map_id_to_location_code_stub), true);

            command::add("map", [](const command::params& params)
            {
                if (params.size() < 2)
                {
                    return;
                }

                const auto name = params.get(1);
                auto location_id = std::atoi(name.data());

                if (location_id == 0)
                {
                    location_id = map_cmd_location_code;
                    const auto location_pack = format_location_package(name);
                    register_builtin_map(map_cmd_id, map_cmd_location_code, name, location_pack);
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
        }

        void start() override
        {
            if (!game::environment::is_mgo())
            {
                return;
            }

            matchmaking::register_callback(matchmaking::event_start_transition, on_start_transition);
            matchmaking::register_callback(matchmaking::event_leave_lobby, on_leave_lobby);
            matchmaking::register_callback(matchmaking::event_match_start, on_match_start);
        }
    };
}

REGISTER_COMPONENT(custom_maps::component)
