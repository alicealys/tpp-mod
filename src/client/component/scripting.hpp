#pragma once

namespace scripting
{
	class lua_lock
	{
	public:
		lua_lock();
		~lua_lock();

		lua_lock(const lua_lock&) = delete;
		lua_lock(lua_lock&&) = delete;

		lua_lock& operator=(const lua_lock&) = delete;
		lua_lock& operator=(lua_lock&&) = delete;

		game::fox::LuaAutoPtr* get_instance();
		game::fox::Lua* get_lua();
		game::lua::lua_State* get_lua_state();

	private:
		game::fox::LuaAutoPtr instance_{};

	};

	class lua_value
	{
	public:
		using variant_t = std::variant<bool, double, std::string>;
		lua_value(game::lua::lua_State* state, int index);

		std::int32_t get_type() const;

		std::string get_string() const;
		double get_number() const;
		bool get_bool() const;

		std::string to_string() const;

	private:
		std::int32_t type_;
		variant_t value_;

	};

	std::unique_ptr<lua_lock> acquire_lock();

	std::optional<lua_value> script_exec(const std::string& code);
}
