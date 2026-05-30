#pragma once

#include "ui_element.hpp"

namespace lui
{
	class ui_timer;
	using ui_timer_ptr = std::shared_ptr<ui_timer>;

	class ui_timer : public ui_element
	{
	public:
		ui_timer();

		static ui_timer_ptr create(const event_t& event, const std::int32_t delay, const bool looping = false);
		static ui_timer_ptr create(const std::string& event, const std::int32_t delay, const bool looping = false);

		void set_event(const event_t& event);
		void set_delay(const std::int32_t delay);
		void set_looping(const bool looping);

	private:
		void update() override;

		std::int32_t delay_{};
		std::int32_t last_fired_{};
		bool looping_{};

		event_t event_{};

	};
}
