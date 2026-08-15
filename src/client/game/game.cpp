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
			game_mode mode;
			bool supported;
		};

		binary_t binaries[mode_count] =
		{
			{"mgsvtpp.exe (eng, 1.0.15.4)", mode_tpp_eng, true},
			{"mgsvmgo.exe (eng, 1.1.2.9)", mode_mgo_eng, true},
			{"mgsvtpp.exe (jpn, 1.0.15.4)", mode_tpp_jpn, false},
			{"mgsvmgo.exe (jpn, 1.1.2.9)", mode_mgo_jpn, false},
		};

		std::unordered_map<std::uint32_t, binary_t> hashes_crc =
		{
			{0x9666C4C1, binaries[mode_tpp_eng]},
			{0x6B4A1707, binaries[mode_mgo_eng]},
			{0x6AF85790, binaries[mode_tpp_jpn]},
			{0x0104AB41, binaries[mode_mgo_jpn]},
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

#ifdef DEBUG
			const auto start = std::chrono::high_resolution_clock::now();
#endif

			std::ifstream stream;
			stream.open(path, std::ios::binary);
			if (!stream.is_open())
			{
				throw std::runtime_error("Failed to get MGSV version");
			}

			std::string buffer;
			buffer.resize(0x10000);

			auto crc_value = crc32(0L, Z_NULL, 0);
			while (stream.is_open() && !stream.eof())
			{
				stream.read(buffer.data(), buffer.size());
				crc_value = crc32(crc_value, reinterpret_cast<std::uint8_t*>(buffer.data()), static_cast<std::uint32_t>(stream.gcount()));
				std::memset(buffer.data(), 0, buffer.size());
			}

#ifdef DEBUG
			const auto end = std::chrono::high_resolution_clock::now();
			const auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			OutputDebugString(utils::string::va("hash: 0x%X, time: %llimsec", crc_value, time));
#endif

			const auto iter = hashes_crc.find(crc_value);
			if (iter == hashes_crc.end())
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
