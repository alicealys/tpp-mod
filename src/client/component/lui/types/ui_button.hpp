#pragma once

#include "ui_element.hpp"

namespace lui
{
	enum button_style_t
	{
		BUTTON_STYLE_DEFAULT = 0,
	};

	using button_action_t = std::variant<std::monostate, std::string, std::function<void()>>;

	struct button_properties_t
	{
		std::uint32_t style;
		std::string text;
		button_action_t action;
	};

	class ui_button;
	using ui_button_ptr = std::shared_ptr<ui_button>;

	class ui_button : public ui_element
	{
	public:
		static ui_button_ptr create(const button_properties_t& properties);

	private:
		void button_style_default(const button_properties_t& properties);

	};
}
