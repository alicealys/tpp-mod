#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "ui_timer.hpp"
#include "../main.hpp"
#include "../../renderer.hpp"

namespace lui
{
	void ui_timer::update()
	{
		auto parent = this->get_parent();
		if (parent == nullptr)
		{
			return;
		}

		const auto now = get_current_msec();
		if (now - this->last_fired_ > this->delay_)
		{
			parent->dispatch_event(this->event_);
			this->last_fired_ = now;

			if (!this->looping_)
			{
				this->close();
			}
		}
	}

	void ui_timer::set_event(const event_t& event)
	{
		this->event_ = event;
	}

	void ui_timer::set_delay(const std::int32_t delay)
	{
		this->delay_ = delay;
	}

	void ui_timer::set_looping(const bool looping)
	{
		this->looping_ = looping;
	}

	ui_timer_ptr ui_timer::create(const event_t& event, const std::int32_t delay, const bool looping)
	{
		auto timer = std::make_shared<ui_timer>();
		timer->event_ = event;
		timer->delay_ = delay;
		timer->looping_ = looping;
		timer->last_fired_ = get_current_msec();
		return timer;
	}

	ui_timer_ptr ui_timer::create(const std::string& name, const std::int32_t delay, const bool looping)
	{
		event_t event{};
		event.name = name;
		return create(event, delay, looping);
	}
}
