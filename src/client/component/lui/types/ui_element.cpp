#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "ui_element.hpp"
#include "../main.hpp"
#include "../input.hpp"

namespace lui
{
	std::int32_t get_current_msec()
	{
		const auto now = std::chrono::steady_clock::now();
		return static_cast<std::int32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
	}
	
	bool is_in_rect(const rect_t& rect, const float x, const float y)
	{
		return x > rect.left && x < rect.right && y > rect.top && y < rect.bottom;
	}

	element_state_t::element_state_t()
	{
		this->position = {};
		this->width = 0.f;
		this->height = 0.f;
		this->color.r = 1.f;
		this->color.g = 1.f;
		this->color.b = 1.f;
		this->color.a = 1.f;
	}

	void ui_element::draw_internal(const draw_info_t& inherit) const
	{

	}

	void ui_element::draw(const draw_info_t& parent_draw_info)
	{
		this->update();

		draw_info_t draw_info{};
		draw_info.alpha = this->animation_state_.current_state.color.a * parent_draw_info.alpha;
		draw_info.rotation = this->animation_state_.current_state.position.rotation + parent_draw_info.rotation;

		this->calculate_rect(parent_draw_info.rect, draw_info.rect);
		this->bounding_rect_ = draw_info.rect;
		this->draw_internal(draw_info);

		for (auto& child : this->children_)
		{
			child->draw(draw_info);
		}

		this->delete_removed_children();
	}

	void ui_element::add_child(ui_element_ptr child)
	{
		if (child.get() == this)
		{
			return;
		}

		child->parent_ = this->shared_from_this();
		this->children_.emplace_back(child);
	}

	void ui_element::remove_child(ui_element_ptr child)
	{
		if (child.get() == this)
		{
			return;
		}

		child->parent_.reset();
		this->children_.remove(child);
	}

	void ui_element::remove_all_children()
	{
		this->children_.clear();
	}

	void ui_element::close()
	{
		this->parent_.reset();
	}

	ui_element_ptr ui_element::get_first_child()
	{
		if (this->children_.empty())
		{
			return nullptr;
		}

		return this->children_.front();
	}
	
	std::list<ui_element_ptr>& ui_element::get_all_children()
	{
		return this->children_;
	}

	ui_element_ptr ui_element::get_last_child()
	{
		if (this->children_.empty())
		{
			return nullptr;
		}

		return this->children_.back();
	}

	void ui_element::delete_removed_children()
	{
		for (auto i = this->children_.begin(); i != this->children_.end(); )
		{
			if (i->get()->get_parent() != this->shared_from_this())
			{
				i = this->children_.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	void ui_element::register_animation_state(const std::string& name, const element_state_t& state)
	{
		this->states_[name] = state;
	}

	void ui_element::animate_to_state(const std::string& name, const std::int32_t duration, const std::uint32_t mode)
	{
		const auto iter = this->states_.find(name);
		if (iter == this->states_.end())
		{
			return;
		}

		this->animation_state_.start_state = this->animation_state_.current_state;
		this->animation_state_.end_state = iter->second;

		this->animation_state_.begin = get_current_msec();
		this->animation_state_.duration = duration;
		this->animation_state_.mode = mode;

		this->update_animation_state();
	}

	void ui_element::update()
	{
		if (this->needs_key_catcher_)
		{
			input::set_key_catcher();
		}

		this->update_animation_state();
	}

	void ui_element::update_animation_state()
	{
		const auto now = get_current_msec();
		auto diff_msec = now - this->animation_state_.begin;

		switch (this->animation_state_.mode)
		{
		case ANIMATE_REPEAT:
			diff_msec %= this->animation_state_.duration;
			break;
		case ANIMATE_LOOP:
		{
			const auto times_over = diff_msec / this->animation_state_.duration;
			const auto mod = diff_msec % this->animation_state_.duration;
			if ((times_over & 1) != 0)
			{
				diff_msec = this->animation_state_.duration - mod;
			}
			else
			{
				diff_msec = mod;
			}
			break;
		}
		}

		auto percent = this->animation_state_.duration > 0
			? std::min(1.f, static_cast<float>(diff_msec) / static_cast<float>(this->animation_state_.duration))
			: 1.f;

#define GET_SAMPLE(__name__) \
		this->animation_state_.current_state.__name__ = \
			this->animation_state_.start_state.__name__ + percent * (this->animation_state_.end_state.__name__ - this->animation_state_.start_state.__name__); \

		GET_SAMPLE(color.r);
		GET_SAMPLE(color.g);
		GET_SAMPLE(color.b);
		GET_SAMPLE(color.a);
		GET_SAMPLE(position.rect.top);
		GET_SAMPLE(position.rect.left);
		GET_SAMPLE(position.rect.left);
		GET_SAMPLE(position.rect.right);
		GET_SAMPLE(position.rect.bottom);
		GET_SAMPLE(position.rotation);
		GET_SAMPLE(width);
		GET_SAMPLE(height);

		if (percent >= 1.f)
		{
			this->animation_state_.current_state.position.anchor = this->animation_state_.end_state.position.anchor;
			this->animation_state_.current_state.position.alignment = this->animation_state_.end_state.position.alignment;
			this->animation_state_.current_state.position.vertical_alignment = this->animation_state_.end_state.position.vertical_alignment;
		}
		else
		{
			this->animation_state_.current_state.position.anchor = this->animation_state_.start_state.position.anchor;
			this->animation_state_.current_state.position.alignment = this->animation_state_.start_state.position.alignment;
			this->animation_state_.current_state.position.vertical_alignment = this->animation_state_.start_state.position.vertical_alignment;
		}
	}

	void ui_element::calculate_rect(const rect_t& parent_rect, rect_t& rect) const
	{
		const auto& current_state = this->animation_state_.current_state;
		const auto& current_pos = current_state.position;

		rect.left = current_pos.rect.left;
		rect.top = current_pos.rect.top;

		if ((current_pos.anchor & ANCHOR_TOP) != 0)
		{
			rect.top += parent_rect.top;
		}
		else if ((current_pos.anchor & ANCHOR_BOTTOM) != 0)
		{
			rect.top = parent_rect.bottom - current_state.height;
		}

		if ((current_pos.anchor & ANCHOR_LEFT) != 0)
		{
			rect.left += parent_rect.left;
		}

		if ((current_pos.anchor & ANCHOR_RIGHT) != 0)
		{
			rect.right = parent_rect.right - current_pos.rect.right;
		}
		else
		{
			rect.right = rect.left + current_state.width;
		}

		if ((current_pos.anchor & ANCHOR_BOTTOM) != 0)
		{
			rect.bottom = parent_rect.bottom - current_pos.rect.bottom;
		}
		else
		{
			rect.bottom = rect.top + current_state.height;
		}
	}

	void ui_element::get_rect(rect_t& rect)
	{
		rect = this->animation_state_.current_state.position.rect;
	}

	void ui_element::set_rect(const rect_t& rect)
	{
		this->animation_state_.end_state.position.rect = rect;
	}

	bool ui_element::handle_mouse_move(const mouse_move_params_t& params, bool has_target)
	{
		for (auto i = this->children_.rbegin(); i != this->children_.rend(); ++i)
		{
			has_target |= i->get()->handle_mouse_move(params, has_target);
		}

		return this->handle_mouse_move_internal(params, has_target);
	}

	bool ui_element::handle_mouse_move_internal(const mouse_move_params_t& params, bool has_target)
	{
		if (!this->mouse_state_.handle_mouse)
		{
			return false;
		}

		const auto was_in = this->mouse_state_.was_mouse_in;
		const auto is_in = !has_target && is_in_rect(this->bounding_rect_, params.x, params.y);
		this->mouse_state_.was_mouse_in = is_in;

		if (is_in)
		{
			has_target = true;
		}

		if (is_in && !was_in)
		{
			event_t event{};
			event.name = "mouseenter";
			this->dispatch_event(event);
		}
		else if (!is_in && was_in)
		{
			event_t event{};
			event.name = "mouseleave";
			this->dispatch_event(event);
		}

		{
			event_t event{};
			event.name = "mousemove";
			event.params[0].value = params.x;
			event.params[1].value = params.y;
			event.params[2].value = params.delta_x;
			event.params[3].value = params.delta_y;
			this->dispatch_event(event);
		}

		return is_in;
	}

	void ui_element::handle_mouse_button(const mouse_button_params_t& params)
	{
		for (auto i = this->children_.rbegin(); i != this->children_.rend(); ++i)
		{
			i->get()->handle_mouse_button(params);
		}

		this->handle_mouse_button_internal(params);
		this->delete_removed_children();
	}

	void ui_element::handle_mouse_button_internal(const mouse_button_params_t& params)
	{
		if (!this->mouse_state_.handle_mouse || !this->mouse_state_.was_mouse_in)
		{
			return;
		}

		event_t event{};
		event.name = params.is_down 
			? "mousedown"
			: "mouseup";
		event.params[0].integer = params.button;
		this->dispatch_event(event);
	}

	void ui_element::set_needs_key_catcher(const bool enabled)
	{
		this->needs_key_catcher_ = enabled;
	}

	void ui_element::set_handle_mouse(const bool enabled)
	{
		this->mouse_state_.handle_mouse = enabled;
	}

	bool ui_element::is_mouse_in() const
	{
		return this->mouse_state_.was_mouse_in;
	}

	void ui_element::dispatch_event(const event_t& event, bool dispatch_children)
	{
		if (dispatch_children)
		{
			for (auto i = this->children_.rbegin(); i != this->children_.rend(); ++i)
			{
				i->get()->dispatch_event(event, dispatch_children);
			}
		}

		this->dispatch_event_internal(event);
	}

	void ui_element::dispatch_event(const std::string& name, bool dispatch_children)
	{
		event_t event{};
		event.name = name;
		this->dispatch_event(event, dispatch_children);
	}
	void ui_element::dispatch_event_internal(const event_t& event)
	{
		const auto handler = this->event_handlers_.find(event.name);
		if (handler == this->event_handlers_.end())
		{
			return;
		}

		handler->second.operator()(*this, event);
	}

	void ui_element::register_event_handler(const std::string& name, const event_handler_t& handler)
	{
		this->event_handlers_[name] = handler;
	}

	ui_element_ptr ui_element::get_parent() const
	{
		return this->parent_.lock();
	}

	void ui_element::set_id(const std::string& id)
	{
		this->id_ = id;
	}

	std::string ui_element::get_id()
	{
		return this->id_;
	}

	ui_element_ptr ui_element::get_first_descendant_by_id(const std::string& id)
	{
		for (auto& child : this->children_)
		{
			if (child->id_ == id)
			{
				return child;
			}
		}

		for (auto& child : this->children_)
		{
			const auto result = child->get_first_descendant_by_id(id);
			if (result != nullptr)
			{
				return result;
			}
		}

		return nullptr;
	}

	ui_element_ptr ui_element::create()
	{
		return std::make_shared<ui_element>();
	}
}
