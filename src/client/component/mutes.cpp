#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "console.hpp"
#include "vars.hpp"
#include "session.hpp"
#include "mutes.hpp"
#include "scheduler.hpp"

#include <utils/io.hpp>
#include <utils/string.hpp>
#include <utils/properties.hpp>
#include <utils/concurrency.hpp>

namespace mutes
{
	namespace
	{
		vars::var_ptr cl_muted_players;

		utils::concurrency::container<mute_list_t, std::recursive_mutex> mute_list;
		std::atomic_bool is_dirty;

		std::unordered_set<std::uint64_t> load_mute_list()
		{
			std::unordered_set<std::uint64_t> list;

			const auto& str = cl_muted_players->current.get_string();
			const auto tokens = utils::string::split(str, ';');
			for (const auto& token : tokens)
			{
				const auto steam_id = std::strtoull(token.data(), nullptr, 0);
				if (steam_id != 0)
				{
					list.insert(steam_id);
				}
			}

			return list;
		}

		void write_mute_list(mute_list_t& list)
		{
			std::string buffer;

			for (const auto& steam_id : list)
			{
				buffer.append(utils::string::va("%lli;", steam_id));
			}

			vars::set_var(cl_muted_players, buffer, vars::var_source_internal);
		}

		bool mute_steam_id(const std::uint64_t steam_id, bool unmute)
		{
			const auto changed = mute_list.access<bool>([&](mute_list_t& list)
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

			is_dirty = changed;

			return changed;
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
			const auto client = session::get_client_by_name(identifier, nullptr, &res_name);
			if (client == nullptr)
			{
				return false;
			}

			mute_steam_id(client->sessionUserId->userId, unmute);

			return true;
		}

		bool mute_player_internal(const std::string& identifier, bool unmute, std::string& res_name)
		{
			if (mute_player_by_steam_id(identifier, unmute, res_name))
			{
				return true;
			}

			return mute_player_by_name(identifier, unmute, res_name);
		}

		utils::hook::detour on_player_mute_hook;
		void on_player_mute_stub(game::mgo_voice_chat_manager_t* manager, unsigned char client_index, char muted)
		{
			const auto client = session::get_client_by_index(client_index, nullptr);
			mute_steam_id(client->sessionUserId->userId, !muted);
			on_player_mute_hook.invoke<void>(manager, client_index, muted);
		}

		void update_muted_players()
		{
			const auto voice_chat_manager = *game::p_mgoVoiceChatManager;
			if (voice_chat_manager == nullptr)
			{
				return;
			}

			mute_list.access([&](mute_list_t& list)
			{
				const auto session = *game::s_pSession;
				if (session != nullptr)
				{
					for (auto i = 0u; i < session->allMembers.size; i++)
					{
						const auto client = session->allMembers.members[i];
						if (client == nullptr)
						{
							continue;
						}

						if (list.contains(client->sessionUserId->userId))
						{
							voice_chat_manager->mutedPlayers |= (1 << i);
						}
						else
						{
							voice_chat_manager->mutedPlayers &= ~(1 << i);
						}
					}
				}

				if (is_dirty)
				{
					write_mute_list(list);
				}
			});
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
			if (!game::environment::is_mgo() || game::environment::is_dedi())
			{
				return;
			}

			cl_muted_players = vars::register_string("cl_muted_players", "", 
				vars::var_flag_saved | vars::var_flag_readonly, "muted player list");

			on_player_mute_hook.create(SELECT_VALUE_LANG(0x1414C86D0, 0x0), on_player_mute_stub);
		}

		void start() override
		{
			if (!game::environment::is_mgo() || game::environment::is_dedi())
			{
				return;
			}

			scheduler::loop(update_muted_players, scheduler::session);

			mute_list.access([&](mute_list_t& list)
			{
				list = load_mute_list();
			});

			command::add("mute", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					console::info("Usage: mute <name|steam_id>\n");
					return;
				}

				const auto session = *game::s_pSession;
				if (session == nullptr)
				{
					console::info("must be in game\n");
					return;
				}

				const auto name = params.join(1);
				scheduler::once([name]
				{
					mute_player(name, false);
				}, scheduler::session);
			});

			command::add("unmute", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					console::info("Usage: unmute <name|steam_id>\n");
					return;
				}

				const auto session = *game::s_pSession;
				if (session == nullptr)
				{
					console::info("must be in game\n");
					return;
				}

				const auto name = params.join(1);
				scheduler::once([name]
				{
					mute_player(name, true);
				}, scheduler::session);
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
					write_mute_list(list);
				});

				console::info("Mute list was cleared\n");
			});
		}
	};
}

REGISTER_COMPONENT(mutes::component)
