#pragma once

#include "game/game.hpp"
#include "object.hpp"
#include "../scripting.hpp"

namespace lui
{
	enum anchor_type_t
	{
		ANCHOR_TOP = (1 << 0),
		ANCHOR_LEFT = (1 << 1),
		ANCHOR_RIGHT = (1 << 2),
		ANCHOR_BOTTOM = (1 << 3),

		ANCHOR_TOPLEFT = ANCHOR_TOP | ANCHOR_LEFT,
		ANCHOR_TOPRIGHT = ANCHOR_TOP | ANCHOR_RIGHT,
		ANCHOR_BOTTOMLEFT = ANCHOR_BOTTOM | ANCHOR_LEFT,
		ANCHOR_BOTTOMRIGHT = ANCHOR_BOTTOM | ANCHOR_RIGHT,

		ANCHOR_ALL = ANCHOR_TOP | ANCHOR_LEFT | ANCHOR_RIGHT | ANCHOR_BOTTOM,
	};

	enum alignment_type_t
	{
		ALIGN_LEFT = 0,
		ALIGN_RIGHT = 1,
		ALIGN_CENTER = 2,

		ALIGN_TOP = 0,
		ALIGN_MIDDLE = 1,
		ALIGN_BOTTOM = 2
	};

	enum animation_mode_t
	{
		ANIMATE_NORMAL = 0,
		ANIMATE_REPEAT = 1,
		ANIMATE_LOOP = 2,
	};

	struct rect_t
	{
		float top;
		float left;
		float right;
		float bottom;
	};

	struct position_t
	{
		std::uint32_t anchor;
		std::uint32_t alignment;
		std::uint32_t vertical_alignment;
		rect_t rect;
		float rotation;
	};

	struct color_t
	{
		float r;
		float g;
		float b;
		float a;
	};

	struct element_state_t
	{
		element_state_t();

		position_t position{};
		float width{};
		float height{};
		color_t color{};
	};

	struct draw_info_t
	{
		rect_t rect;
		float alpha;
		float rotation;
	};

	struct animation_state_t
	{
		element_state_t current_state;
		element_state_t start_state;
		element_state_t end_state;
		std::int32_t begin{};
		std::int32_t duration{};
		std::uint32_t mode{};
	};

	struct mouse_move_params_t
	{
		float x{};
		float y{};
		float delta_x{};
		float delta_y{};
	};

	struct mouse_button_params_t
	{
		int button{};
		bool is_down{};
	};

	struct mouse_state_t
	{
		bool handle_mouse{};
		bool blocking = true;
		bool was_mouse_in{};
		bool did_mouse_down{};
	};

	class ui_element;
	using ui_element_ptr = std::shared_ptr<ui_element>;

	struct event_t : public object
	{
		ui_element_ptr target;
		std::string name{};
		bool immediate{};
		bool dispatch_children{};
	};

	using event_handler_t = std::function<void(ui_element&, const event_t&)>;

	std::int32_t get_current_msec();
	bool is_in_rect(const rect_t& rect, const float x, const float y);

	class ui_element : public std::enable_shared_from_this<ui_element>
	{
		friend class ui_button;
		friend class ui_image;
		friend class ui_list;
		friend class ui_menu;
		friend class ui_text;
		friend class ui_timer;
	public:
		ui_element();

		void draw(const draw_info_t& inherit);

		void add_child(ui_element_ptr child);
		void remove_child(ui_element_ptr child);
		void remove_all_children();
		void close();

		ui_element_ptr get_first_child();
		std::list<ui_element_ptr>& get_all_children();
		ui_element_ptr get_last_child();

		void register_animation_state(const std::string& name, const element_state_t& state);
		void animate_to_state(const std::string& name, const std::int32_t duration = 0, const std::uint32_t = ANIMATE_NORMAL);

		void get_rect(rect_t& rect);
		void get_client_rect(rect_t& rect);
		void set_rect(const rect_t& rect);
		void set_color(const float r, const float g, const float b, const float a);

		bool handle_mouse_move(const mouse_move_params_t& params, bool has_target = false);
		void handle_mouse_button(const mouse_button_params_t& params);

		void dispatch_event(const event_t& event);
		void dispatch_event(const std::string& name, bool immediate = true, bool dispatch_children = false);
		void register_event_handler(const std::string& name, const event_handler_t& handler);
		void run_queued_events();

		void set_needs_key_catcher(const bool enabled);
		void set_handle_mouse(const bool enabled);
		void set_mouse_blocking(const bool enabled);
		bool is_mouse_in() const;
		bool is_mouse_down() const;

		void make_draggable(const std::optional<ui_element_ptr>& target_opt = {});

		void get_animation_state(const std::string& name, element_state_t& state, bool& found) const;
		void get_current_animation_state(element_state_t& state) const;

		ui_element_ptr get_parent() const;

		void set_id(const std::string& id);
		std::string get_id();
		ui_element_ptr get_first_descendant_by_id(const std::string& id);

		static ui_element_ptr create();

		object metadata{};
		sol::table lua_metadata;

	private:
		virtual void update();
		void update_animation_state();
		virtual void draw_internal(const draw_info_t& inherit) const;
		void calculate_rect(const rect_t& parent_rect, rect_t& rect) const;
		void delete_removed_children();

		bool handle_mouse_move_internal(const mouse_move_params_t& params, bool has_target = false);
		void handle_mouse_button_internal(const mouse_button_params_t& params);

		void dispatch_event_internal(const event_t& event);

		std::list<ui_element_ptr> children_{};
		std::weak_ptr<ui_element> parent_{};

		std::unordered_map<std::string, element_state_t> states_{};
		std::unordered_map<std::string, event_handler_t> event_handlers_{};

		std::vector<event_t> event_queue_{};

		animation_state_t animation_state_{};
		mouse_state_t mouse_state_{};
		rect_t client_rect_{};
		bool needs_key_catcher_{};

		std::string id_{};

	};
}
