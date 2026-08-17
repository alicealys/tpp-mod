#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "custom_maps.hpp"
#include "console.hpp"
#include "matchmaking.hpp"

#include "text_chat/defs.hpp"
#include "text_chat/ui.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace map_rotation
{
	namespace
	{
		struct slot_field_t
		{
			std::string name;
			std::size_t offset;
		};

		std::vector<game::match_slot_t> map_rotation;
		std::size_t rotation_index = 0;

		std::vector<slot_field_t> match_slot_fields =
		{
			{"m_match_rule", offsetof(game::match_slot_t, m_match_rule)},
			{"m_map_id", offsetof(game::match_slot_t, m_map_id)},
			{"m_variant", offsetof(game::match_slot_t, m_variant)},
			{"m_dn", offsetof(game::match_slot_t, m_dn)},
			{"m_time_limit", offsetof(game::match_slot_t, m_time_limit)},
			{"m_tickets", offsetof(game::match_slot_t, m_tickets)},
			{"m_unique_char", offsetof(game::match_slot_t, m_unique_char)},
			{"m_unique_char_solid", offsetof(game::match_slot_t, m_unique_char_solid)},
			{"m_unique_char_liquid", offsetof(game::match_slot_t, m_unique_char_liquid)},
			{"m_walker_gear", offsetof(game::match_slot_t, m_walker_gear)},
			{"m_weather_change", offsetof(game::match_slot_t, m_weather_change)},
		};

		void set_slot_field(game::match_slot_t& slot, const std::size_t offset, const int value)
		{
			const auto base = reinterpret_cast<std::size_t>(&slot);
			*reinterpret_cast<char*>(base + offset) = static_cast<char>(value);
		}

		utils::hook::detour next_map_hook;
		int next_map_stub(game::mgo_match_t* match, int a2)
		{
			if (map_rotation.empty() || map_rotation.size() <= 5)
			{
				return next_map_hook.invoke<int>(match, a2);
			}

			const auto next_index = (match->match_rules.pl_current_match + 1) % 5;
			console::debug("[map rotation] next_index: %i\n", next_index);
			if (next_index != 0)
			{
				return next_map_hook.invoke<int>(match, a2);
			}

			const auto count = std::min(5ull, map_rotation.size());
			auto match_idx = (match->match_rules.pl_total_match + 1) % map_rotation.size();

			console::debug("[map rotation] next map: id: %i, rule: %i\n", map_rotation[match_idx].m_map_id, map_rotation[match_idx].m_match_rule);

			for (auto i = 0; i < count; i++)
			{
				std::memcpy(&match->match_rules.slots[i], &map_rotation[match_idx], sizeof(game::match_slot_t));
				match_idx++;
				if (match_idx >= map_rotation.size())
				{
					match_idx = 0;
				}
			}

			for (auto i = 0; i < 5; i++)
			{
				console::debug("[map rotation] slot %i: id: %i, rule: %i\n", i, 
					match->match_rules.slots[i].m_map_id, match->match_rules.slots[i].m_match_rule);
			}

			return next_map_hook.invoke<int>(match, a2);
		}

		game::match_slot_t& current_slot()
		{
			if (map_rotation.empty())
			{
				game::match_slot_t slot{};
				slot.m_match_rule = 2;
				slot.m_tickets = 30;
				slot.m_time_limit = 5;
				map_rotation.emplace_back(slot);
			}

			const auto index = map_rotation.size() - 1;
			return map_rotation[index];
		}
	}

	void start_rotation(game::mgo_match_t* match)
	{
		if (map_rotation.empty())
		{
			return;
		}

		const auto count = std::min(map_rotation.size(), 5ull);

		for (auto i = 0; i < count; i++)
		{
			std::memcpy(&match->match_settings.rules.slots[i], &map_rotation[i], sizeof(game::match_slot_t));
		}

		match->match_settings.rules.pl_match_num = static_cast<char>(count);
		match->match_settings.rules.pl_current_match = 0;
		match->match_settings.rules.pl_total_match = 0;
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			if (!game::environment::is_mgo() || !game::environment::is_dedi())
			{
				return;
			}

			command::add("matchrotationclear", []
			{
				map_rotation.clear();
			});

			command::add("matchaddslot", []()
			{
				game::match_slot_t slot{};
				slot.m_match_rule = 2;
				slot.m_tickets = 30;
				slot.m_time_limit = 5;
				map_rotation.emplace_back(slot);
			});

			for (const auto& field : match_slot_fields)
			{
				command::add(field.name, [&](const command::params& params)
				{
					if (params.size() < 2)
					{
						return;
					}

					auto& slot = current_slot();
					set_slot_field(slot, field.offset, params.get_int(1));
				});
			}

			command::add("m_map_name", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					return;
				}

				auto& slot = current_slot();
				slot.m_map_id = custom_maps::get_map_id(params.get(1));
			});
		}

		void start() override
		{
			if (!game::environment::is_mgo() || !game::environment::is_dedi())
			{
				return;
			}

			next_map_hook.create(SELECT_VALUE_LANG(0x1405A6BF0, 0x0), next_map_stub);
		}
	};
}

REGISTER_COMPONENT(map_rotation::component)
