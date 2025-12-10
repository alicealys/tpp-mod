#pragma once

#include "structs.hpp"

#define SELECT_VALUE(...) game::select(__VA_ARGS__)
#define SELECT_VALUE_NOLANG(tpp, mgo) (game::environment::is_tpp() ? (tpp) : (mgo))
#define SELECT_VALUE_LANG(eng, jpn) (game::environment::is_eng() ? (eng) : (jpn))

namespace game
{
	namespace environment
	{
		enum game_mode
		{
			mode_none = -1,
			mode_tpp_eng = 0,
			mode_mgo_eng = 1,
			mode_tpp_jpn = 2,
			mode_mgo_jpn = 3,
			mode_count = 4,
		};

		game_mode get_mode();

		void set_mode(game_mode mode);

		bool is_tpp();
		bool is_mgo();
		bool is_dedi();
		bool is_eng();
		bool is_jpn();

		void detect_version();
	}

	template <typename ...Args, size_t N = sizeof...(Args)>
	auto select(Args... values)
	{
		using T = std::common_type_t<Args...>;
		const std::array<T, environment::mode_count> v = {values...};
		const auto mode = environment::get_mode();
		assert(mode >= 0);
		return v[mode];
	}

	template <typename T>
	class symbol
	{
	public:
		symbol(const size_t tpp_eng, const size_t mgo_eng, const size_t tpp_jpn, const size_t mgo_jpn)
		{
			this->addresses_[environment::mode_tpp_eng] = tpp_eng;
			this->addresses_[environment::mode_mgo_eng] = mgo_eng;
			this->addresses_[environment::mode_tpp_jpn] = tpp_jpn;
			this->addresses_[environment::mode_mgo_jpn] = mgo_jpn;
		}

		T* get() const
		{
			return reinterpret_cast<T*>(this->addresses_[environment::get_mode()]);
		}

		operator T* () const
		{
			return this->get();
		}

		T* operator->() const
		{
			return this->get();
		}

	private:
		std::array<size_t, environment::mode_count> addresses_{};

	};
}

#include "symbols.hpp"
