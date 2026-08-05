#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "mods.hpp"
#include "console.hpp"
#include "scheduler.hpp"
#include "filesystem.hpp"
#include "scripting.hpp"
#include "matchmaking.hpp"
#include "custom_maps.hpp"

#include "utils/steam.hpp"

#include <utils/hook.hpp>
#include <utils/nt.hpp>
#include <utils/io.hpp>
#include <utils/http.hpp>
#include <utils/cryptography.hpp>
#include <utils/string.hpp>
#include <utils/concurrency.hpp>

namespace mods
{
    vars::var_ptr var_fs_mod_path;
    vars::var_ptr var_sv_base_url;

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
            bool has_info;
            std::uint32_t flags;
            std::string name;
            std::string author;
            std::string version;
            std::optional<std::string> path;
            std::vector<mod_pack_file_t> pack_files;
        };

        mod_info_t current_mod{};

        enum mod_download_state_t
        {
            download_state_none = 0,
            download_state_ask_begin = 1,
            download_state_ask_wait = 2,
            download_state_ask_end = 3,
            download_state_start = 4,
            download_state_dl_begin= 5,
            download_state_dl_wait = 6,
            download_state_dl_cancel = 7,
            download_state_error = 8,
            download_state_end = 9,
        };

        struct
        {
            std::mutex mutex;
            std::string mod_path;
            std::atomic_bool result;
            std::atomic_uint32_t state;
            std::vector<mod_download_file_t> files;
            std::string server_url;
            game::steam_id target_lobby;
        } mod_download_state{};

        struct hash_job_t
        {
            std::string path;
            std::atomic_bool done;
            std::function<void(const std::uint32_t)> callback;
        };

        using hash_job_list_t = std::deque<std::unique_ptr<hash_job_t>>;
        std::atomic_size_t hash_job_count = 0;
        utils::concurrency::container<hash_job_list_t> hash_jobs;

        void parse_mod_info(const std::string& data, mod_info_t& info)
        {
            auto mod_info_j = nlohmann::json::parse(data, nullptr, false);
            if (mod_info_j.is_discarded())
            {
                return;
            }

            info.has_info = true;

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

        void register_mod_search_paths(const std::string& path, const bool add)
        {
            if (add)
            {
                filesystem::register_path(path);
            }
            else
            {
                filesystem::unregister_path(path);
            }
        }

        void clear_mod(bool& needs_restart)
        {
            if (current_mod.path.has_value())
            {
                const auto& path = current_mod.path.value();
                console::info("[Mods] Unloading mod: %s\n", path.data());
                register_mod_search_paths(path, false);
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

            register_mod_search_paths(normal_path, true);

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

            const auto match_container = game::s_mgoMatchMakingManager->match_container;
            if (match_container != nullptr && match_container->match != nullptr && match_container->match->lobby_id.bits != 0)
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
                console::debug("[FS Module] create mount point: %s %s %lli %i\n", type, name, a3, flags);
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

        void mod_download_ask_popup_update()
        {
            if (mod_download_state.state < download_state_ask_begin || mod_download_state.state > download_state_ask_end)
            {
                return;
            }

            const auto is_showing_popup = scripting::script_exec("return TppUiCommand.IsShowPopup(\"mod_download_ask_popup\")");
            if (is_showing_popup.has_value() && is_showing_popup->is_true())
            {
                if (mod_download_state.state == download_state_ask_begin)
                {
                    mod_download_state.state = download_state_ask_wait;
                }
                return;
            }
            else if (mod_download_state.state != download_state_ask_wait)
            {
                return;
            }

            const auto result = scripting::script_exec("return TppUiCommand.GetPopupSelect()");
            if (!result.has_value() || !result->is_number() || static_cast<int>(result->get_number()) != 1)
            {
                mod_download_state.state = download_state_ask_end;
            }
            else
            {
                mod_download_state.state = download_state_start;
                scripting::script_exec(
                    "TppUiCommand.ErasePopup(); TppUiCommand.ShowPopup(\"mod_download_progress_popup\", 0);"
                );
            }
        }
        
        void mod_download_progress_popup_update()
        {
            const auto is_showing_popup = scripting::script_exec("return TppUiCommand.IsShowPopup(\"mod_download_progress_popup\")");
            if (is_showing_popup.has_value() && is_showing_popup->is_true())
            {
                if (mod_download_state.state == download_state_dl_begin)
                {
                    mod_download_state.state = download_state_dl_wait;
                }

                mod_download_file_t* current_file = nullptr;
                auto index = 0;
                for (auto& file : mod_download_state.files)
                {
                    if (!file.done)
                    {
                        current_file = &file;
                        break;
                    }

                    ++index;
                }

                const auto now = std::chrono::system_clock::now().time_since_epoch();
                const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
                const auto dot_count = std::clamp((now_ms % 1000) / 250, 0ll, 3ll);
                char dots[5] = "...";
                dots[dot_count] = 0;

                std::string script;
                if (current_file != nullptr)
                {
                    script = std::format("TppUiCommand.SetPopupText(\"Downloading {} ({}/{}) {}\")", 
                        current_file->path, index + 1, mod_download_state.files.size(), dots);
                }
                else
                {
                    script = std::format("TppUiCommand.SetPopupText(\"Done.\")");
                }

                scripting::script_exec(script);
                return;
            }
            else if (mod_download_state.state != download_state_dl_wait)
            {
                return;
            }

            mod_download_state.state = download_state_dl_cancel;
        }

        void reset_mod_download()
        {
            std::lock_guard lock(mod_download_state.mutex);
            mod_download_state.state = download_state_none;
            mod_download_state.files.clear();
            mod_download_state.server_url.clear();
            mod_download_state.mod_path.clear();
            mod_download_state.result = false;
            mod_download_state.target_lobby.bits = 0;
        }

        void mod_download_update_async()
        {
            switch (mod_download_state.state)
            {
            case download_state_dl_begin:
            case download_state_dl_wait:
            {
                auto all_done = true;
                for (auto& file : mod_download_state.files)
                {
                    if (file.done)
                    {
                        continue;
                    }

                    all_done = false;
                    const auto data = utils::http::get_data(file.url);
                    if (mod_download_state.state != download_state_dl_begin && 
                        mod_download_state.state != download_state_dl_wait)
                    {
                        return;
                    }

                    if (!data.has_value())
                    {
                        console::error("[Mods] failed to download mod file %s, no data from server\n", file.path.data());
                        mod_download_state.state = download_state_error;
                        return;
                    }

                    const auto hash = utils::cryptography::crc32::compute(data.value());
                    console::debug("[Mods] download file hash %X == %X\n", file.hash, hash);
                    if (hash != file.hash)
                    {
                        console::error("[Mods] failed to download mod file %s, hash mismatch\n", file.path.data());
                        mod_download_state.state = download_state_error;
                        return;
                    }

                    std::lock_guard lock(mod_download_state.mutex);
                    file.data = data.value();
                    file.done = true;
                }

                if (all_done)
                {
                    mod_download_state.state = download_state_end;
                    mod_download_state.result = true;
                }
            }
            }
        }

        void mod_download_update()
        {
            static auto prev_state = mod_download_state.state.load();
            if (prev_state != mod_download_state.state)
            {
                prev_state = mod_download_state.state;
                console::debug("[Mods] mod_download_state: %i\n", prev_state);
            }

            switch (mod_download_state.state)
            {
            case download_state_none:
                return;
            case download_state_ask_begin:
            case download_state_ask_wait:
                mod_download_ask_popup_update();
                break;
            case download_state_ask_end:
                mod_download_state.state = download_state_none;
                break;
            case download_state_start:
                mod_download_state.state = download_state_dl_begin;
                break;
            case download_state_dl_begin:
            case download_state_dl_wait:
            {
                mod_download_progress_popup_update();
                break;
            }
            case download_state_dl_cancel:
                mod_download_state.result = false;
                mod_download_state.state = download_state_end;
                break;
            case download_state_error:
                scripting::script_exec(
                    "TppUiCommand.ErasePopup();"
                    "TppUiCommand.ShowPopup(\"mod_download_error\");"
                    "TppUiCommand.SetPopupText(\"Failed to download mod, check console for details\");");
                mod_download_state.state = download_state_none;
                console::debug("[Mods] mod download error!\n");
                reset_mod_download();
                break;
            case download_state_end:
                scripting::script_exec("TppUiCommand.ErasePopup();");
                mod_download_state.state = download_state_none;
                const bool result = mod_download_state.result.load();
                console::debug("[Mods] mod download result: %i\n", result);

                for (auto& file : mod_download_state.files)
                {
                    const auto hash_file = file.path + ".hash";
                    if (!utils::io::write_file(file.path, file.data) || 
                        !utils::io::write_file(hash_file, std::to_string(file.hash)))
                    {
                        console::error("[Mods] failed to write file %s\n", file.path.data());
                        mod_download_state.state = download_state_error;
                        break;
                    }
                }

                scripting::script_exec("TppUiCommand.StopMbDvcTerminal()");

                const auto target = mod_download_state.target_lobby;
                if (mod_download_state.mod_path.empty() && !current_mod.path.has_value())
                {
                    scheduler::once([=]
                    {
                        matchmaking::connect_to_lobby(target);
                    }, scheduler::main, 100ms);
                }
                else
                {
                    if (mod_download_state.mod_path.empty())
                    {
                        mods::unload();
                    }
                    else
                    {
                        mods::load(mod_download_state.mod_path);
                    }

                    scheduler::once([target]
                    {
                        matchmaking::connect_to_lobby(target);
                    }, scheduler::main, 3s);
                }

                reset_mod_download();
                break;
            }
        }

        bool check_download_mod(const game::steam_id lobby_id, 
            std::string& new_fs_mod, 
            std::string& error, 
            std::string& base_url,
            std::vector<mod_download_file_t>& files)
        {
            base_url = utils::steam::get_lobby_data<std::string>(lobby_id, "mod_base_url", -1, 256);
            if (!base_url.ends_with("/"))
            {
                base_url.push_back('/');
            }

            const auto server_mod = utils::steam::get_lobby_data<std::string>(lobby_id, "mod_path", -1, 32);
            if (server_mod.empty())
            {
                new_fs_mod = "";
                return false;
            }

            if (base_url.empty())
            {
                error = "Server has a mod, but has no URL to download it from!";
                return false;
            }

            if (!base_url.starts_with("http://") && !base_url.starts_with("https://"))
            {
                error = "Server has a mod, but has an invalid URL to download it from!";
                return false;
            }

            if (!server_mod.starts_with("mods/") || server_mod.contains('.') || server_mod.contains("::"))
            {
                error = std::format("Invalid server mod value \"{}\"", server_mod);
                return false;
            }

            const auto num_files = utils::steam::get_lobby_data<std::int32_t>(lobby_id, "mod_file_num");
            if (num_files < 0 || num_files > 8)
            {
                error = std::format("Server has too many mod files ({})", num_files);
                return false;
            }

            new_fs_mod = server_mod;

            static std::unordered_set<std::string> allowed_file_types =
            {
                {".json"},
                {".dat"},
            };

            for (auto i = 0; i < num_files; i++)
            {
                const auto file_name = utils::steam::get_lobby_data<std::string>(lobby_id, "mod_file_name", i, 32);
                const auto file_hash = utils::steam::get_lobby_data<std::uint32_t>(lobby_id, "mod_file_hash", i);

                if (!validate_file_name(file_name) || file_hash == 0)
                {
                    error = "Invalid mod filename";
                    return false;
                }

                const auto dot_pos = file_name.find_last_of('.');
                if (dot_pos == std::string::npos)
                {
                    error = "Invalid mod filename";
                    return false;
                }

                const auto ext = file_name.substr(dot_pos);
                if (!allowed_file_types.contains(ext))
                {
                    error = "Invalid mod file type";
                    return false;
                }

                mod_download_file_t file{};
                file.path = std::format("{}/{}", server_mod, file_name);
                file.hash = file_hash;
                file.url = base_url + file.path;

                const auto hash_file = file.path + ".hash";
                if (utils::io::file_exists(file.path) && utils::io::file_exists(hash_file))
                {
                    const auto hash_txt = utils::io::read_file(hash_file);
                    const auto hash = std::strtoul(hash_txt.data(), nullptr, 0);

                    if (hash != file.hash)
                    {
                        files.emplace_back(file);
                    }
                }
                else
                {
                    files.emplace_back(file);
                }
            }

            return !files.empty();
        }

        void on_lobby_create(game::mgo_match_t* match, const game::steam_id lobby_id)
        {
            utils::steam::set_lobby_data(lobby_id, "mod_base_url", var_sv_base_url->current.get_string());
            custom_maps::on_lobby_create(match, lobby_id);

            if (!current_mod.path.has_value())
            {
                return;
            }

            utils::steam::set_lobby_data(lobby_id, "mod_path", current_mod.path.value());

            auto count = 0;
            const auto add_file = [&](const std::string& name)
            {
                const auto path = std::format("{}\\{}", current_mod.path.value(), name);
                const auto data = utils::io::read_file(path);
                auto index = count++;

                add_hash_job(path, [lobby_id, index](const std::uint32_t hash)
                {
                    utils::steam::set_lobby_data(lobby_id, "mod_file_hash", hash, index);
                });

                utils::steam::set_lobby_data(lobby_id, "mod_file_name", name, index);
            };

            if (current_mod.has_info)
            {
                add_file("mod.json");
            }

            for (auto& packfile : current_mod.pack_files)
            {
                add_file(packfile.name);
            }

            utils::steam::set_lobby_data(lobby_id, "mod_file_num", count);
        }

        void run_hash_jobs()
        {
            const auto job = hash_jobs.access<std::unique_ptr<hash_job_t>>([](hash_job_list_t& list)
                -> std::unique_ptr<hash_job_t>
            {
                if (list.empty())
                {
                    return nullptr;
                }

                std::unique_ptr<hash_job_t> job = std::move(list.front());
                list.pop_front(); 
                return job;
            });

            if (job.get() == nullptr)
            {
                return;
            }

            std::ifstream stream(job->path, std::ios::binary);
        
            std::string buffer;
            buffer.resize(0x10000);
            
            auto crc_value = crc32(0L, Z_NULL, 0);
            while (stream.is_open() && !stream.eof())
            {
                stream.read(buffer.data(), buffer.size());
                crc_value = crc32(crc_value, reinterpret_cast<std::uint8_t*>(buffer.data()), static_cast<std::uint32_t>(stream.gcount()));
                std::memset(buffer.data(), 0, buffer.size());
            }

            console::debug("hash job completed: %X\n", crc_value);
            job->callback(crc_value);
            --hash_job_count;
        }

        bool wait_for_hash_jobs_stub(__int64 a1)
        {
            const auto result = utils::hook::invoke<char>(SELECT_VALUE_LANG(0x1403708E0, 0x0), a1);
            if (!matchmaking::is_host())
            {
                return result;
            }

            console::debug("hashes left: %i\n", hash_job_count.load());
            return result || hash_job_count > 0;
        }
    }

    bool try_download_mod(game::mgo_match_t* match, const game::steam_id lobby_id)
    {
        const auto self_id = matchmaking::get_current_steam_id();
        if (match->error1 != 17 || match->lobby_owner.bits == self_id.bits)
        {
            return false;
        }

        custom_maps::sync_registered_maps(lobby_id);
        reset_mod_download();

        std::string new_fs_mod;
        std::string error;
        std::string base_url;

        auto needs_download = false;
        needs_download |= check_download_mod(lobby_id, new_fs_mod, error, base_url, mod_download_state.files);
        needs_download |= custom_maps::check_download_map(lobby_id, error, base_url, mod_download_state.files);

        if (needs_download)
        {
            match->error1 = 30;
            match->error2 = 0x8800000D;

            mod_download_state.target_lobby = lobby_id;
            mod_download_state.mod_path = new_fs_mod;
            mod_download_state.state = download_state_ask_begin;

            scripting::script_exec(std::format(
                "TppUiCommand.ErasePopup();"
                "TppUiCommand.ShowPopup(\"mod_download_ask_popup\", Popup.TYPE_TWO_BUTTON);"
                "TppUiCommand.SetPopupText(\"This server is running a mod, accept the download (url: {})?\")", base_url));

            return true;
        }
        else if (!error.empty())
        {
            match->error1 = 30;
            match->error2 = 0x8800000D;

            scripting::script_exec(std::format(
                "TppUiCommand.ErasePopup();"
                "TppUiCommand.ShowPopup(\"mod_download_error_popup\");"
                "TppUiCommand.SetPopupText(\"ERROR: {}\")", error));

            return true;
        }
        else if (new_fs_mod != var_fs_mod_path->latched.get_string())
        {
            match->error1 = 30;
            match->error2 = 0x8800000D;

            const auto post_load_cmd = std::format("connect_lobby {}", lobby_id.bits);
            const auto arg = std::format("+{}", post_load_cmd);

            if (new_fs_mod.empty())
            {
                mods::unload(arg);
            }
            else
            {
                mods::load(new_fs_mod, arg);
            }

            scheduler::once([=]
            {
                command::execute(post_load_cmd);
            }, scheduler::main, 3s);

            return true;
        }

        return false;
    }

    bool validate_file_name(const std::string& name)
    {
        static const char* invalid_str[] =
        {
            "..",
            ":",
            "\\",
            "/",
            "^",
            "?",
            "!",
        };

        if (name.empty())
        {
            return false;
        }

        for (auto i = 0; i < ARRAYSIZE(invalid_str); i++)
        {
            const auto pos = name.find(invalid_str[i]);
            if (pos != std::string::npos)
            {
                return false;
            }
        }

        return true;
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

    void unload(const std::optional<std::string>& arg)
    {
        auto needs_restart = false;

        clear_mod(needs_restart);
        vars::set_var(var_fs_mod_path, "", vars::var_source_internal);

        if (needs_restart)
        {
            auto cmd = std::format("{}", arg.value_or(""));
            utils::nt::relaunch_self("", true);
            utils::nt::terminate();
            return;
        }

        reload_assets();
    }

    void add_hash_job(const std::string& path, const std::function<void(const std::uint32_t)>& callback)
    {
        auto hash_job = std::make_unique<hash_job_t>();
        hash_job->path = path;
        hash_job->done = false;
        hash_job->callback = callback;

        hash_jobs.access([&](hash_job_list_t& list)
        {
            console::debug("add hash job %s\n", hash_job->path.data());
            hash_job_count++;
            list.emplace_back(std::move(hash_job));
        });
    }

    class component final : public component_interface
    {
    public:
        void pre_load() override
        {
            var_fs_mod_path = vars::register_string("fs_mod", "", vars::var_flag_latched, "mod folder path");

            if (game::environment::is_mgo())
            {
                var_sv_base_url = vars::register_string("sv_base_url", "", vars::var_flag_saved, "base url for server mod downloading (sent to lobby members)");
                utils::hook::call(SELECT_VALUE_LANG(0x140893B9F, 0x0), wait_for_hash_jobs_stub);
            }

            fs_module_init_hook.create(SELECT_VALUE(0x14003A960, 0x14003A830, 0x0, 0x0), fs_module_init_stub);
            create_pack_mountpoint_hook.create(game::fox::fs::MountPoint_::CreateWithPackFile, create_pack_mountpoint_stub);
        }

        void post_load() override
        {
            const auto& fs_mod = var_fs_mod_path->latched.get_string();
            if (!fs_mod.empty())
            {
                register_mod_search_paths(fs_mod, true);
            }
        }

        void start() override
        {
            if (!game::environment::is_mgo())
            {
                return;
            }

            matchmaking::register_callback(matchmaking::event_join_lobby, try_download_mod);
            matchmaking::register_callback(matchmaking::event_create_lobby, on_lobby_create);

            scheduler::loop(mod_download_update, scheduler::main);
            scheduler::loop(mod_download_update_async, scheduler::async);
            scheduler::loop(run_hash_jobs, scheduler::async);

            command::add("unloadmod", []()
            {
                if (var_fs_mod_path->latched.get_string().empty())
                {
                    console::warn("no mod loaded\n");
                    return;
                }

                if (!can_load_mod())
                {
                    console::warn("cannot load mods right now");
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
