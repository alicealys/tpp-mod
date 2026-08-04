#pragma once

#include "vars.hpp"
#include "game/game.hpp"

namespace mods
{
    struct mod_download_file_t
    {
        bool done;
        std::string url;
        std::string path;
        std::uint32_t hash;
        std::string data;
    };

    extern vars::var_ptr var_fs_mod_path;

    game::fox::fs::MountPoint* add_packfile(const std::string& path, const std::string& type, const std::uint32_t flags);
    game::fox::fs::MountPoint* add_packfile(const std::string& path, const std::uint64_t type, const std::uint32_t flags);
    void load(const std::string& path, const std::optional<std::string>& arg = {});
    void unload(const std::optional<std::string>& arg = {});

    bool try_download_mod(game::mgo_match_t* match, const game::steam_id lobby_id);
    bool validate_file_name(const std::string& name);

    void add_hash_job(const std::string& path, const std::function<void(const std::uint32_t)>& callback);
}
