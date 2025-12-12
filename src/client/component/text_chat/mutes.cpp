#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "../command.hpp"
#include "../console.hpp"
#include "mutes.hpp"

#include <utils/io.hpp>
#include <utils/string.hpp>
#include <utils/properties.hpp>
#include <utils/concurrency.hpp>

namespace text_chat::mutes
{
	namespace
	{
		utils::concurrency::container<mute_list_t, std::recursive_mutex> mute_list;

		std::string get_mute_list_path()
		{
			const auto path = utils::properties::get_appdata_path() / "mutes.txt";
			return path.generic_string();
		}

		std::unordered_set<std::uint64_t> load_mute_list()
		{
			std::unordered_set<std::uint64_t> list;

			std::string data;
			if (utils::io::read_file(get_mute_list_path(), &data))
			{
				const auto lines = utils::string::split_lines(data);
				for (const auto& line : lines)
				{
					const auto steam_id = std::strtoull(line.data(), nullptr, 0);

					if (steam_id != 0ull)
					{
						list.insert(steam_id);
					}
				}
			}

			return list;
		}

		void write_mute_list()
		{
			mute_list.access([&](mute_list_t& list)
			{
				std::string buffer;

				for (const auto& steam_id : list)
				{
					buffer.append(utils::string::va("%lli\n", steam_id));
				}

				utils::io::write_file(get_mute_list_path(), buffer);
			});
		}

		bool mute_steam_id(const std::uint64_t steam_id, bool unmute)
		{
			const auto _0 = gsl::finally(write_mute_list);

			return mute_list.access<bool>([&](mute_list_t& list)
			{
				const auto was_present = list.contains(steam_id);

				if (unmute)
				{
					list.erase(steam_id);
					return was_present;
				}
				else
				{
					list.insert(steam_id);
					return !was_present;
				}
			});
		}

		bool mute_player_by_steam_id(const std::string& identifier, bool unmute, std::string& name)
		{
			const auto steam_id = std::strtoull(identifier.data(), nullptr, 0);
			if (steam_id == 0)
			{
				return false;
			}

			mute_steam_id(steam_id, unmute);
			name = utils::string::va("%lli", steam_id);

			return true;
		}

		bool mute_player_by_name(const std::string& identifier, bool unmute, std::string& res_name)
		{
			const auto match_container = game::s_mgoMatchMakingManager->match_container;
			if (match_container == nullptr)
			{
				return false;
			}

			const auto steam_matchmaking = (*game::SteamMatchmaking)();
			const auto steam_friends = (*game::SteamFriends)();

			const auto lower = utils::string::to_lower(identifier);

			const auto num_players = steam_matchmaking->__vftable->GetNumLobbyMembers(steam_matchmaking, match_container->match->lobby_id);
			for (auto i = 0; i < num_players; i++)
			{
				game::steam_id steam_id{};
				steam_matchmaking->__vftable->GetLobbyMemberByIndex(steam_matchmaking, &steam_id, match_container->match->lobby_id, i);
				const std::string name = steam_friends->__vftable->GetFriendPersonaName(steam_friends, steam_id);
				const auto target_name_lower = utils::string::to_lower(name);

				if (target_name_lower.starts_with(lower))
				{
					res_name = name;
					mute_steam_id(steam_id.bits, unmute);
					return true;
				}
			}

			return false;
		}

		bool mute_player_internal(const std::string& identifier, bool unmute, std::string& res_name)
		{
			if (mute_player_by_steam_id(identifier, unmute, res_name))
			{
				return true;
			}

			return mute_player_by_name(identifier, unmute, res_name);
		}
	}

	void mute_player(const std::string& identifier, bool unmute)
	{
		std::string name;
		if (!mute_player_internal(identifier, unmute, name))
		{
			console::info("Player not found");
		}
		else
		{
			console::info("%s has been %s", name.data(), unmute ? "unmuted" : "muted");
		}
	}

	bool is_muted(const std::uint64_t steam_id)
	{
		return mute_list.access<bool>([&](mute_list_t& list)
		{
			return list.contains(steam_id);
		});
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{

		}

		void start() override
		{
			if (!game::environment::is_mgo())
			{
				return;
			}

			mute_list.access([&](mute_list_t& list)
			{
				list = load_mute_list();
			});

			command::add("mute", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					console::info("Usage: mute <name|steam_id>", false);
					return;
				}

				mute_player(params.join(1), false);
			});

			command::add("unmute", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					console::info("Usage: unmute <name|steam_id>", false);
					return;
				}

				mute_player(params.join(1), true);
			});

			command::add("mutelist", [](const command::params& params)
			{
				const auto steam_friends = (*game::SteamFriends)();

				mute_list.access([&](mute_list_t& list)
				{
					for (const auto& steam_id : list)
					{
						game::steam_id steam_id_{};
						steam_id_.bits = steam_id;

						const auto name = steam_friends->__vftable->GetFriendPersonaName(steam_friends, steam_id_);
						console::info("%s (%lli)\n", name, steam_id);
					}
				});
			});

			command::add("clearmutes", [](const command::params& params)
			{
				mute_list.access([&](mute_list_t& list)
				{
					list.clear();
					write_mute_list();
				});

				console::info("Mute list was cleared", false);
			});
		}
	};
}

REGISTER_COMPONENT(text_chat::mutes::component)
