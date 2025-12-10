#include <std_include.hpp>
#include "game.hpp"

#include <utils/flags.hpp>
#include <utils/nt.hpp>
#include <utils/cryptography.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>

namespace game
{
	namespace environment
	{
		struct binary_t
		{
			std::string name;
			game::environment::game_mode mode;
			bool supported;
		};

		std::unordered_map<std::string, binary_t> binaries =
		{
			{"17AC94A4BC9F88B035A45122C8A67EFA38D03F2A", {"mgsvtpp.exe (eng, 1.0.15.3)", mode_tpp_eng, true}},
			{"8690C7C27C94DD6A452BA6A612B7B485918F3BAF", {"mgsvmgo.exe (eng, 1.1.2.7)", mode_mgo_eng, true}},
			{"3A87F626732158890A07688D32A2523CD8EADA71", {"mgsvtpp.exe (jpn, 1.0.15.3)", mode_tpp_jpn, false}},
			{"D6ADF7685B0F0639B2A949D0E96A06E853DAEEB8", {"mgsvmgo.exe (jpn, 1.1.2.7)", mode_mgo_jpn, false}},
		};

		bool initialized = false;
		game_mode mode = game_mode::mode_none;

		void set_mode(const game_mode mode_)
		{
			mode = mode_;
		}

		game_mode get_mode()
		{
			return mode;
		}

		bool is_tpp()
		{
			return get_mode() == mode_tpp_eng || get_mode() == mode_tpp_jpn;
		}

		bool is_mgo()
		{
			return get_mode() == mode_mgo_eng || get_mode() == mode_mgo_jpn;
		}

		bool is_dedi()
		{
			static const auto res = is_mgo() && utils::flags::has_flag("dedicated");
			return res;
		}

		bool is_eng()
		{
			return get_mode() == mode_tpp_eng || get_mode() == mode_mgo_eng;
		}

		bool is_jpn()
		{
			return get_mode() == mode_tpp_jpn || get_mode() == mode_mgo_jpn;
		}

		void detect_version()
		{
			utils::nt::library self;
			const auto path = self.get_path();
			std::string data;

			if (!utils::io::read_file(path, &data))
			{
				throw std::runtime_error("Failed to get MGSV version");
			}

			const auto hash = utils::cryptography::sha1::compute(data, true);
			const auto iter = binaries.find(hash);

			OutputDebugString(hash.data());

			if (iter == binaries.end())
			{
				throw std::runtime_error("Unknown MGSV version");
			}

			if (!iter->second.supported)
			{
				throw std::runtime_error(utils::string::va("This version of MGSV is not supported:\n%s", iter->second.name.data()));
			}

			set_mode(iter->second.mode);
		}
	}
}
