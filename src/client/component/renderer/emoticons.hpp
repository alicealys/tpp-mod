#pragma once

#include "game/game.hpp"

namespace renderer
{
	constexpr const auto max_emoticon_length = 32;

	enum emoticon_game_t : std::uint32_t
	{
		emoticon_game_both = 0,
		emoticon_game_tpp = 1,
		emoticon_game_mgo = 2,
	};

	enum emoticon_flags_t
	{
		emoticon_flag_force_color = (1 << 0),
		emoticon_flag_mask = (1 << 1),
	};

	struct emoticon_t
	{
		std::string name;
		std::uint32_t flags;
		game::fox::gr::Material* material{};
		float color[4]{};
		float ratio{};
	};

	bool get_emoticon(const char* text, unsigned int& len, emoticon_t** emoticon);
	bool get_emoticon(const wchar_t* text, unsigned int& len, emoticon_t** emoticon);

	float draw_emoticon(game::fox::gr::dg::plugins::Draw2DRenderer* instance, emoticon_t* emoticon, float x, float y, float size, float alpha, bool add_color = true, float* color_override = nullptr);
	
	void find_emoticons(const std::string& input, std::vector<emoticon_t*>& matches);
}
