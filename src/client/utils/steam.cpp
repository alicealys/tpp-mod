#include <std_include.hpp>

#include "steam.hpp"

#include <utils/string.hpp>

namespace utils::steam
{
    namespace
    {
        const char* get_key(const char* key, const std::int32_t index)
        {
            if (index != -1)
            {
                return utils::string::va("%s_%i", key, index);
            }
            else
            {
                return key;
            }
        }
    }

    template <>
    const char* get_lobby_data(const game::steam_id lobby_id, const char* key, const int index, const std::int32_t)
    {
        const auto steam_matchmaking = (*game::SteamMatchmaking)();
        const auto key_str = get_key(key, index);
        const auto value = steam_matchmaking->__vftable->GetLobbyData(steam_matchmaking, lobby_id, key_str);
        return value;
    };

    template <>
    std::int32_t get_lobby_data(const game::steam_id lobby_id, const char* key, const std::int32_t index, const std::int32_t)
    {
        const auto value = get_lobby_data<const char*>(lobby_id, key, index);
        return std::atoi(value);
    };

    template <>
    bool get_lobby_data(const game::steam_id lobby_id, const char* key, const std::int32_t index, const std::int32_t)
    {
        return static_cast<bool>(get_lobby_data<std::int32_t>(lobby_id, key, index));
    };

    template <>
    std::int64_t get_lobby_data(const game::steam_id lobby_id, const char* key, const std::int32_t index, const std::int32_t)
    {
        const auto value = get_lobby_data<const char*>(lobby_id, key, index);
        return std::strtoll(value, nullptr, 0);
    };

    template <>
    std::uint32_t get_lobby_data(const game::steam_id lobby_id, const char* key, const std::int32_t index, const std::int32_t)
    {
        const auto value = get_lobby_data<const char*>(lobby_id, key, index);
        return std::strtoul(value, nullptr, 0);
    };

    template <>
    std::uint64_t get_lobby_data(const game::steam_id lobby_id, const char* key, const std::int32_t index, const std::int32_t)
    {
        const auto value = get_lobby_data<const char*>(lobby_id, key, index);
        return std::strtoull(value, nullptr, 0);
    };

    template <>
    std::string get_lobby_data(const game::steam_id lobby_id, const char* key, const std::int32_t index, const std::int32_t limit)
    {
        std::string data = get_lobby_data<const char*>(lobby_id, key, index);
        if (data.size() > limit)
        {
            data.resize(limit);
        }
        return data;
    };

    void set_lobby_data(const game::steam_id lobby_id, const char* key, const char* value, const std::int32_t index)
    {
        const auto steam_matchmaking = (*game::SteamMatchmaking)();
        const auto key_str = get_key(key, index);
        steam_matchmaking->__vftable->SetLobbyData(steam_matchmaking, lobby_id, key_str, value);
    }

    void set_lobby_data(const game::steam_id lobby_id, const char* key, const std::string& value, const std::int32_t index)
    {
        set_lobby_data(lobby_id, key, value.data(), index);
    }
}
