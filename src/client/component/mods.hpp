#pragma once

#include "vars.hpp"

namespace mods
{
    extern vars::var_ptr var_fs_mod_path;

    void load(const std::string& path, const std::optional<std::string>& arg = {});
    void unload();
}
