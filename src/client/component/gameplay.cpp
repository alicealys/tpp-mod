#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "console.hpp"
#include "command.hpp"
#include "binds.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace gameplay
{
	namespace
	{
		game::tpp::gm::player::PlayerSlotType get_slot_type(const std::string& name)
		{
			static std::unordered_map<std::string, game::tpp::gm::player::PlayerSlotType> slot_types =
			{
				{"primary1", game::tpp::gm::player::PlayerSlotType::PRIMARY_1},
				{"primary2", game::tpp::gm::player::PlayerSlotType::PRIMARY_2},
				{"secondary", game::tpp::gm::player::PlayerSlotType::SECONDARY},
				{"support", game::tpp::gm::player::PlayerSlotType::SUPPORT},
				{"item", game::tpp::gm::player::PlayerSlotType::ITEM},
				{"stole", game::tpp::gm::player::PlayerSlotType::STOLE},
				{"hand", game::tpp::gm::player::PlayerSlotType::HAND},
			};

			const auto lower = utils::string::to_lower(name);
			const auto iter = slot_types.find(lower);
			if (iter != slot_types.end())
			{
				return iter->second;
			}

			return game::tpp::gm::player::PlayerSlotType::PRIMARY_1;
		}

		bool is_player_initialized()
		{
			if (game::environment::is_tpp())
			{
				return game::tpp::gm::player::player2System->player2System != nullptr &&
					game::tpp::gm::player::player2System->player2System->tpp.controller != nullptr;
			}
			else
			{
				return game::tpp::gm::player::player2System->player2System != nullptr &&
					game::tpp::gm::player::player2System->player2System->mgo.controller != nullptr;
			}
		}

		std::uint8_t get_current_index_for_slot_type(const game::tpp::gm::player::PlayerSlotType slot_type)
		{
			const auto player = game::tpp::gm::player::player2System->player2System;
			if (game::environment::is_tpp())
			{
				switch (slot_type)
				{
				case game::tpp::gm::player::PlayerSlotType::SUPPORT:
					return player->tpp.controller->__vftable->GetCurrentSupportIndex(player->tpp.controller, player->tpp.localPlayerIndex);
				case game::tpp::gm::player::PlayerSlotType::ITEM:
					return player->tpp.controller->__vftable->GetCurrentItemIndex(player->tpp.controller, player->tpp.localPlayerIndex);
				}
			}
			else
			{
				switch (slot_type)
				{
				case game::tpp::gm::player::PlayerSlotType::SUPPORT:
					return player->mgo.controller->__vftable->GetCurrentSupportIndex(player->mgo.controller, player->mgo.localPlayerIndex);
				case game::tpp::gm::player::PlayerSlotType::ITEM:
					return player->mgo.controller->__vftable->GetCurrentItemIndex(player->mgo.controller, player->mgo.localPlayerIndex);
				}
			}

			return 0;
		}

		void set_current_slot(const game::tpp::gm::player::PlayerSlotType slot_type, const std::uint8_t sub_index)
		{
			const auto player = game::tpp::gm::player::player2System->player2System;
			if (game::environment::is_tpp())
			{
				player->tpp.controller->__vftable->SetCurrentSlot(player->tpp.controller, player->tpp.localPlayerIndex, slot_type, sub_index, 0, 1, -2);
			}
			else
			{
				player->mgo.controller->__vftable->SetCurrentSlot(player->mgo.controller, player->mgo.localPlayerIndex, slot_type, sub_index, 0, 1, -2);
			}
		}

		std::uint8_t get_max_slots(const game::tpp::gm::player::PlayerSlotType slot_type)
		{
			if (game::environment::is_tpp())
			{
				switch (slot_type)
				{
				case game::tpp::gm::player::PlayerSlotType::SUPPORT:
				case game::tpp::gm::player::PlayerSlotType::ITEM:
					return 8u;
				default:
					return 1u;
				}
			}
			else
			{
				switch (slot_type)
				{
				case game::tpp::gm::player::PlayerSlotType::SUPPORT:
				case game::tpp::gm::player::PlayerSlotType::ITEM:
					return 4u;
				default:
					return 1u;
				}
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			binds::add_action_command("equiptoggle", [](const command::params& params)
			{
				if (!is_player_initialized())
				{
					return;
				}

				const auto slot_type = get_slot_type(params.get(1));
				const auto sub_index_first = static_cast<std::uint8_t>(params.get_int(2) & 0xF);
				const auto sub_index_second = static_cast<std::uint8_t>(params.get_int(3) & 0xF);
				const auto current_index = get_current_index_for_slot_type(slot_type);

				if (current_index == sub_index_first)
				{
					set_current_slot(slot_type, sub_index_second);
				}
				else
				{
					set_current_slot(slot_type, sub_index_first);
				}
			});

			binds::add_action_command("equipnext", [](const command::params& params)
			{
				if (!is_player_initialized())
				{
					return;
				}

				const auto slot_type = get_slot_type(params.get(1));
				const auto max_set = static_cast<std::uint8_t>(params.get_int(2));
				auto current_index = get_current_index_for_slot_type(slot_type);
				const auto max = std::min(max_set, get_max_slots(slot_type));
				
				if (current_index >= max - 1)
				{
					current_index = 0;
				}
				else
				{
					++current_index;
				}

				current_index = std::min(current_index, max);
				set_current_slot(slot_type, current_index);
			});

			binds::add_action_command("equipprev", [](const command::params& params)
			{
				if (!is_player_initialized())
				{
					return;
				}

				const auto slot_type = get_slot_type(params.get(1));
				const auto max_set = static_cast<std::uint8_t>(params.get_int(2));

				auto current_index = get_current_index_for_slot_type(slot_type);
				const auto max = std::min(max_set, get_max_slots(slot_type));

				if (current_index == 0)
				{
					current_index = max - 1;
				}
				else
				{
					--current_index;
				}

				current_index = std::min(current_index, max);

				set_current_slot(slot_type, current_index);
			});

			binds::add_action_command("equip", [](const command::params& params)
			{
				if (!is_player_initialized())
				{
					return;
				}

				const auto slot_type = get_slot_type(params.get(1));
				const auto sub_index = static_cast<std::uint8_t>(params.get_int(2) & 0xF);

				set_current_slot(slot_type, sub_index);
			});
		}

		void start() override
		{

		}
	};
}

REGISTER_COMPONENT(gameplay::component)
