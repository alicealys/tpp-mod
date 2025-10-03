#pragma once

#include "structs.hpp"

#define SELECT_VALUE(tpp, mgo) (game::environment::is_tpp() ? (tpp) : (mgo))

namespace game
{
	namespace environment
	{
		enum game_mode
		{
			none,
			tpp,
			mgo,
			dedi,
		};

		game_mode get_mode();
		void set_mode(game_mode mode);

		bool is_tpp();
		bool is_mgo();
		bool is_dedi();
	}

	template <typename T>
	class symbol
	{
	public:
		symbol(const size_t tpp, const size_t mgo)
		{
			if (environment::is_tpp())
			{
				this->address_ = reinterpret_cast<T*>(tpp);
			}
			else
			{
				this->address_ = reinterpret_cast<T*>(mgo);
			}
		}

		T* get() const
		{
			return this->address_;
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
		T* address_{};

	};
}

#include "symbols.hpp"
