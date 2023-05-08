#pragma once

#include "structs.hpp"

#define SELECT_VALUE(mgsv, mgo) (game::environment::is_mgsv() ? (mgsv) : (mgo))

namespace game
{
	namespace environment
	{
		enum game_mode
		{
			none,
			mgsv,
			mgo,
		};

		game_mode get_mode();
		void set_mode(game_mode mode);

		bool is_mgsv();
		bool is_mgo();
	}

	template <typename T>
	class symbol
	{
	public:
		symbol(const size_t mgsv, const size_t mgo)
			: mgsv_address_(reinterpret_cast<T*>(mgsv))
			  , mgo_address_(reinterpret_cast<T*>(mgo))
		{
		}

		T* get() const
		{
			if (environment::is_mgsv())
			{
				return reinterpret_cast<T*>(this->mgsv_address_);
			}

			return this->mgo_address_;
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
		T* mgsv_address_{};
		T* mgo_address_{};

	};
}

#include "symbols.hpp"
