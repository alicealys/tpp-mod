#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "filesystem.hpp"
#include "backend_server.hpp"
#include "scheduler.hpp"
#include "console.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>

namespace online_news
{
	namespace
	{
		std::mutex mutex;
		std::unordered_set<std::uint32_t> viewed_info_ids;
		std::atomic_bool is_dirty;

		bool should_show_info_id_original(unsigned int info_id)
		{
			if (game::environment::is_tpp())
			{
				const auto script_vars = &game::fox::GetQuarkSystemTable()->applicationSystem->tpp.scriptVars->tpp;
				return info_id > script_vars->infoId;
			}
			else
			{
				const auto script_vars = &game::fox::GetQuarkSystemTable()->applicationSystem->mgo.scriptVars->mgo;
				return info_id > script_vars->infoId;
			}
		}

		bool should_show_info_id(unsigned int info_id)
		{
			if (!backend_server::is_using_custom_server())
			{
				return should_show_info_id_original(info_id);
			}

			std::lock_guard _0(mutex);
			return !viewed_info_ids.contains(info_id);
		}

		std::string get_storage_path()
		{
			const auto base_path = backend_server::get_data_folder();
			return std::format("{}\\viewed_info_ids_{}.txt", base_path, std::uint8_t(game::environment::is_mgo()));
		}

		void ui_online_news_controller_refresh_indices_stub(utils::hook::assembler& a)
		{
			auto show_news = a.new_label();

			a.push(rax);
			a.pushad64();
			a.mov(ecx, qword_ptr(rsi, -0x14));
			a.call_aligned(should_show_info_id);
			a.mov(qword_ptr(rsp, 0x80), rax);
			a.popad64();
			a.pop(rax);

			a.cmp(al, 0);
			a.jnz(show_news);

			a.jmp(SELECT_VALUE(0x14096E9A1, 0x1406DB501, 0x0, 0x0));

			a.bind(show_news);
			a.jmp(SELECT_VALUE(0x14096E9B8, 0x1406DB518, 0x0, 0x0));
		}

		utils::hook::detour ui_online_news_controller_save_infoid_hook;
		void ui_online_news_controller_save_infoid_stub(game::tpp::ui::menu::impl::UiOnlineNewsControllerImpl* news)
		{
			ui_online_news_controller_save_infoid_hook.invoke<void>(news);

			if (!backend_server::is_using_custom_server())
			{
				return;
			}

			std::lock_guard _0(mutex);

			for (auto i = 0u; i < news->count; i++)
			{
				const auto index = news->indices[i];
				const auto info = news->infos[index];

				if (!viewed_info_ids.contains(info.id))
				{
					viewed_info_ids.insert(info.id);
					is_dirty = true;
				}
			}
		}

		void parse_info_ids()
		{
			std::string data;
			if (!utils::io::read_file(get_storage_path(), &data))
			{
				return;
			}

			const auto lines = utils::string::split_lines(data);

			std::lock_guard _0(mutex);
			for (const auto& line : lines)
			{
				const auto info_id = std::strtoul(line.data(), nullptr, 0);
				viewed_info_ids.insert(info_id);
			}
		}

		void save_info_ids()
		{
			if (!is_dirty)
			{
				return;
			}

			std::string buffer;

			{
				std::lock_guard _0(mutex);
				for (auto& id : viewed_info_ids)
				{
					buffer.append(std::format("{}\n", id));
				}
			}

			if (!utils::io::write_file(get_storage_path(), buffer))
			{
				console::error("[online news] failed to store viewed info ids\n");
			}

			is_dirty = false;
		}
	}

	class component final : public component_interface
	{
	public:
		void pre_load() override
		{
			utils::hook::jump(SELECT_VALUE(0x14096E995, 0x1406DB4F5, 0x0, 0x0),
				utils::hook::assemble(ui_online_news_controller_refresh_indices_stub), true);
			ui_online_news_controller_save_infoid_hook.create(SELECT_VALUE(0x14096EC50, 0x1406DB7B0, 0x0, 0x0),
				ui_online_news_controller_save_infoid_stub);
		}

		void game_initialized() override
		{
			if (!backend_server::is_using_custom_server())
			{
				return;
			}

			parse_info_ids();
			scheduler::loop(save_info_ids, scheduler::async);
		}
	};
}

REGISTER_COMPONENT(online_news::component)
