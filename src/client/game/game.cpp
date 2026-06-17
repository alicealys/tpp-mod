#include <std_include.hpp>
#include "game.hpp"

#include <utils/flags.hpp>
#include <utils/nt.hpp>
#include <utils/cryptography.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>

#define HASH_MD5

namespace game
{
	namespace environment
	{
		struct binary_t
		{
			std::string name;
			game_mode mode;
			bool supported;
		};

		binary_t binaries[mode_count] =
		{
			{"mgsvtpp.exe (eng, 1.0.15.4)", mode_tpp_eng, true},
			{"mgsvmgo.exe (eng, 1.1.2.8)", mode_mgo_eng, false},
			{"mgsvtpp.exe (jpn, 1.0.15.4)", mode_tpp_jpn, false},
			{"mgsvmgo.exe (jpn, 1.1.2.8)", mode_mgo_jpn, false},
		};

		std::unordered_map<std::string, binary_t> hashes_sha1 =
		{
			{"D763B9B6CC1DB749B45B0FED2742F56571AE8FFA", binaries[mode_tpp_eng]},
			{"C6F6108DB0A6FFF5BEACFAB6773B0CB192527CE7", binaries[mode_mgo_eng]},
			{"54BC743D1D7D98678EF4CC8D5BC98439F92AA1F7", binaries[mode_tpp_jpn]},
			{"505BBF337DED2DFDD4F86FAAA2F4A9DC3CE935B0", binaries[mode_mgo_jpn]},
		};

		std::unordered_map<std::string, binary_t> hashes_md5 =
		{
			{"5362E39F6FC9CC330059379A9A602649", binaries[mode_tpp_eng]},
			{"8E13DE0EDA115A702AA096A8E85FD129", binaries[mode_mgo_eng]},
			{"E6BB25CDDEE36849F5F53237746F68DE", binaries[mode_tpp_jpn]},
			{"EB3EC459B1CF9199D01088A545433340", binaries[mode_mgo_jpn]},
		};

		std::unordered_map<std::string, game_mode> gamemodes =
		{
			{"tppeng", mode_tpp_eng},
			{"mgoeng", mode_mgo_eng},
			{"tppjpn", mode_tpp_jpn},
			{"mgojpn", mode_mgo_jpn},
		};

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

		void manual_version(const std::string& value)
		{
			const auto iter = gamemodes.find(value);
			if (iter == gamemodes.end())
			{
				throw std::runtime_error(utils::string::va("invalid gamemode \"%s\", options: tppeng, mgoeng, tppjpn, mgojpn", value.data()));
			}

			set_mode(iter->second);
		}

		void detect_version()
		{
			utils::nt::library self;
			const auto path = self.get_path();
			std::string data;

			const auto override_ = utils::flags::get_flag("mode");
			if (override_.has_value())
			{
				manual_version(override_.value());
				return;
			}

			if (!utils::io::read_file(path, &data))
			{
				throw std::runtime_error("Failed to get MGSV version");
			}

#ifdef DEBUG
			const auto start = std::chrono::high_resolution_clock::now();
#endif

#ifdef HASH_MD5
			const auto& hashes = hashes_md5;
			const auto hash = utils::cryptography::md5::compute(data, true);
#elifdef HASH_SHA1
			const auto& hashes = hashes_sha1;
			const auto hash = utils::cryptography::sha1::compute(data, true);
#endif

#ifdef DEBUG
			const auto end = std::chrono::high_resolution_clock::now();
			const auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			OutputDebugString(utils::string::va("hash: %s, time: %llimsec", hash.data(), time));
#endif

			const auto iter = hashes.find(hash);
			if (iter == hashes.end())
			{
				throw std::runtime_error("Unknown MGSV version, use -mode <gamemode> to manually specify it (tppeng, mgoeng, tppjpn, mgojpn)");
			}

			if (!iter->second.supported)
			{
				throw std::runtime_error(utils::string::va("This version of MGSV is not supported:\n%s", iter->second.name.data()));
			}

			set_mode(iter->second.mode);
		}
	}
}
