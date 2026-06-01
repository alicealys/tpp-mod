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

	event_t::event_t(const std::string& name)
	{
		this->name = name;
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

#ifdef DEBUG
	auto total_elems = 0;
#endif

	ui_element::ui_element()
	{
		this->id_ = "uielement";
#ifdef DEBUG
		console::debug("+ create ui element (%i)\n", ++total_elems);
#endif
	}

	ui_element::~ui_element()
	{
#ifdef DEBUG
		console::debug("- delete ui element (%i, %s)\n", --total_elems, this->id_.data());
#endif
		this->parent_.reset();
		this->children_.clear();
	}

	void ui_element::track()
	{
		track_element(this->shared_from_this());
	}

	void ui_element::draw_internal(const draw_info_t& inherit) const
	{
	}

	void ui_element::draw(const draw_info_t& parent_draw_info)
	{
		this->run_queued_events();
		this->update();

		draw_info_t draw_info{};
		draw_info.alpha = this->animation_state_.current_state.color.a * parent_draw_info.alpha;
		draw_info.rotation = this->animation_state_.current_state.position.rotation + parent_draw_info.rotation;

		this->calculate_rect(parent_draw_info.rect, draw_info.rect);
		this->client_rect_ = draw_info.rect;
		this->draw_internal(draw_info);

		for (auto& child : this->children_)
		{
			child->draw(draw_info);
		}

		this->delete_removed_children();
	}

	void ui_element::add_child(ui_element_ptr child)
	{
		if (child.get() == this || child->get_parent() != nullptr)
		{
			return;
		}

		child->parent_ = this->shared_from_this();
		this->children_.emplace_back(child);
	}

	void ui_element::remove_child(ui_element_ptr child)
	{
		if (child.get() == this || child->parent_.lock() != this->shared_from_this())
		{
			return;
		}

		child->parent_.reset();
	}

	void ui_element::remove_all_children()
	{
		for (auto& child : this->children_)
		{
			this->remove_child(child);
		}
	}

	void ui_element::close(bool recurse)
	{
		if (recurse)
		{
			for (auto& child : this->children_)
			{
				child->close();
			}
		}

		this->parent_.reset();
		this->event_handlers_.clear();
		this->event_queue_.clear();
		this->children_.clear();
		this->metadata = {};
		this->lua_metadata = {};
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

	void ui_element::get_client_rect(rect_t& rect)
	{
		rect = this->client_rect_;
	}

	void ui_element::set_rect(const rect_t& rect)
	{
		this->animation_state_.start_state.position.rect = rect;
		this->animation_state_.end_state.position.rect = rect;
	}

	void ui_element::set_color(const float r, const float g, const float b, const float a)
	{
		this->animation_state_.start_state.color.r = r;
		this->animation_state_.start_state.color.g = g;
		this->animation_state_.start_state.color.b = b;
		this->animation_state_.start_state.color.a = a;
		this->animation_state_.end_state.color.r = r;
		this->animation_state_.end_state.color.g = g;
		this->animation_state_.end_state.color.b = b;
		this->animation_state_.end_state.color.a = a;
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
		const auto is_in = !has_target && is_in_rect(this->client_rect_, params.x, params.y);
		this->mouse_state_.was_mouse_in = is_in;

		if (is_in && !was_in)
		{
			static event_t event("mouseenter");
			event.target = this->shared_from_this();
			event.immediate = true;
			event.dispatch_children = false;

			this->dispatch_event(event);
		}
		else if (!is_in && was_in)
		{
			static event_t event("mouseleave");
			event.target = this->shared_from_this();
			event.immediate = true;
			event.dispatch_children = false;

			this->dispatch_event(event);
		}

		{
			static event_t event("mousemove");

			event.target = this->shared_from_this();
			event.immediate = true;
			event.dispatch_children = false;

			event.params.set("x", params.x);
			event.params.set("y", params.y);
			event.params.set("deltax", params.delta_x);
			event.params.set("deltay", params.delta_y);

			this->dispatch_event(event);
		}

		return is_in && this->mouse_state_.blocking;
	}

	void ui_element::set_mouse_blocking(const bool enabled)
	{
		this->mouse_state_.blocking = enabled;
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
		if (!this->mouse_state_.handle_mouse)
		{
			return;
		}

		if (params.is_down && this->mouse_state_.was_mouse_in)
		{
			static event_t event("mousedown");
			event.immediate = true;
			event.name = "mousedown";
			event.target = this->shared_from_this();
			event.params.set("button", params.button);

			this->dispatch_event(event);
			this->mouse_state_.did_mouse_down = true;
		}
		else if (!params.is_down && (this->mouse_state_.was_mouse_in || this->mouse_state_.did_mouse_down))
		{
			static event_t event("mouseup");
			event.immediate = true;
			event.target = this->shared_from_this();
			event.params.set("button", params.button);

			this->dispatch_event(event);
			this->mouse_state_.did_mouse_down = false;
		}
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

	bool ui_element::is_mouse_down() const
	{
		return this->mouse_state_.did_mouse_down;
	}

	void ui_element::make_draggable(const std::optional<ui_element_ptr>& target_opt)
	{
		this->set_handle_mouse(true);
		this->set_needs_key_catcher(true);

		this->register_event_handler("mousemove", [=](ui_element& element, const event_t& event)
		{
			auto target = target_opt.value_or(element.shared_from_this());
			if (!element.is_mouse_down())
			{
				return;
			}

			const auto delta_x = event.params.get<float>("deltax");
			const auto delta_y = event.params.get<float>("deltay");

			rect_t rect{};
			target->get_rect(rect);

			rect.left += delta_x;
			rect.top += delta_y;
			rect.right -= delta_x;
			rect.bottom -= delta_y;

			target->set_rect(rect);

			static event_t next_event("drag");
			next_event.target = target;
			next_event.dispatch_children = false;
			next_event.immediate = true;

			next_event.params.set("left", rect.left);
			next_event.params.set("top", rect.top);
			next_event.params.set("right", rect.right);
			next_event.params.set("bottom", rect.bottom);

			target->dispatch_event(next_event);
		});
	}

	void ui_element::dispatch_event(const event_t& event)
	{
		if (!event.immediate)
		{
			this->event_queue_.emplace_back(event);
			return;
		}

		if (event.dispatch_children)
		{
			for (auto i = this->children_.rbegin(); i != this->children_.rend(); ++i)
			{
				i->get()->dispatch_event(event);
			}
		}

		this->dispatch_event_internal(event);
	}

	void ui_element::dispatch_event(const std::string& name, bool immediate, bool dispatch_children)
	{
		static event_t event{};
		event.target = this->shared_from_this();
		event.name = name;
		event.immediate = immediate;
		event.dispatch_children = dispatch_children;
		this->dispatch_event(event);
	}

	void ui_element::dispatch_event_internal(const event_t& event)
	{
		const auto handler = this->event_handlers_.find(event.name);
		if (handler == this->event_handlers_.end())
		{
			return;
		}

		try
		{
			auto target = event.target.lock();
			handler->second.operator()(*target, event);
		}
		catch (const std::exception& e)
		{
			console::error("LUI: error handling event \"%s\": %s\n", event.name.data(), e.what());
		}
	}

	void ui_element::run_queued_events()
	{
		for (auto& event : this->event_queue_)
		{
			event.immediate = true;
			this->dispatch_event(event);
		}

		this->event_queue_.clear();
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

	void ui_element::get_animation_state(const std::string& name, element_state_t& state, bool& found) const
	{
		const auto iter = this->states_.find(name);
		if (iter != this->states_.end())
		{
			found = true;
			state = iter->second;
		}
		else
		{
			found = false;
		}
	}

	void ui_element::get_current_animation_state(element_state_t& state) const
	{
		state = this->animation_state_.end_state;
	}

	ui_element_ptr ui_element::create()
	{
		const auto element = std::make_shared<ui_element>();
		element->track();
		return element;
	}
}
