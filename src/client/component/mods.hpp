#pragma once

#include "vars.hpp"
#include "game/game.hpp"

namespace mods
{
    extern vars::var_ptr var_fs_mod_path;

    game::fox::fs::MountPoint* add_packfile(const std::string& path, const std::string& type, const std::uint32_t flags);
    game::fox::fs::MountPoint* add_packfile(const std::string& path, const std::uint64_t type, const std::uint32_t flags);
    void load(const std::string& path, const std::optional<std::string>& arg = {});
    void unload(const std::optional<std::string>& arg = {});
}
