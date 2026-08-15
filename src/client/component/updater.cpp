#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "console.hpp"
#include "scheduler.hpp"

#include <utils/http.hpp>
#include <version.h>

namespace updater
{
	namespace
	{
		void check_for_updates()
		{
			utils::http::headers headers;
			headers["user-agent"] = "curl";

			const auto url = "https://api.github.com/repos/alicealys/tpp-mod/commits/main";
			const auto data = utils::http::get_data(url, headers);
			if (!data.has_value())
			{
				return;
			}

			auto commit_info_j = nlohmann::json::parse(data.value(), nullptr, false);
			if (commit_info_j.is_discarded() || !commit_info_j.is_object())
			{
				return;
			}

			auto& sha_j = commit_info_j["sha"];
			if (!sha_j.is_string())
			{
				return;
			}

			const auto sha = sha_j.get<std::string>();
			if (GIT_HASH != sha)
			{
				console::warn("********************************\n");
				console::warn("A TPP-Mod update is available!\nDownload it from:\nhttp://github.alicent.cat/tpp-mod/latest/dinput8.dll\n");
				console::warn("********************************\n");
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
#if GIT_DIRTY == 0 and not defined(DEBUG)
			scheduler::once(check_for_updates, scheduler::async);
#endif
		}
	};
}

REGISTER_COMPONENT(updater::component)
