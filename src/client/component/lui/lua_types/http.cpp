#include <std_include.hpp>

#include "http.hpp"
#include "json.hpp"
#include "../../console.hpp"
#include "../../backend_server.hpp"

#include <utils/http.hpp>
#include <utils/string.hpp>

namespace lui::scripting::http
{
	namespace
	{
		struct http_request_t
		{
			sol::unsafe_function callback;
			std::future<std::optional<std::string>> result;
		};

		struct server_http_request_t
		{
			sol::unsafe_function callback;
			std::future<std::optional<nlohmann::json>> result;
		};

		template <typename T>
		struct handler_t
		{
			std::deque<T> requests;
			std::deque<T> new_requests;
		};

		handler_t<http_request_t> http_handler;
		handler_t<server_http_request_t> server_handler;
	}

	void setup(sol::state& state)
	{
		state["http"] = state.create_table();

		state["http"]["get"] = [](const std::string& url, const sol::unsafe_function& callback)
		{
			http_request_t request{};
			request.callback = callback;
			request.result = utils::http::get_data_async(url);
			http_handler.new_requests.emplace_back(std::move(request));
		};

		state["http"]["sendcommand"] = sol::overload(
			[](const std::string& endpoint, const nlohmann::json& data_j, const sol::unsafe_function& callback)
			{
				const auto use_session = endpoint == "WEB";
				server_http_request_t request{};
				request.callback = callback;

				request.result = std::async(std::launch::async, [endpoint, data_j, use_session]()
				{
					return backend_server::send_command(endpoint, data_j, use_session);
				});

				server_handler.new_requests.emplace_back(std::move(request));
			},
			[](const std::string& endpoint, const sol::table& data, const sol::unsafe_function& callback)
			{
				const auto use_session = endpoint == "WEB";
				server_http_request_t request{};
				request.callback = callback;
				const auto data_j = json::lua_to_json(data);

				request.result = std::async(std::launch::async, [endpoint, data_j, use_session]()
				{
					return backend_server::send_command(endpoint, data_j, use_session);
				});

				server_handler.new_requests.emplace_back(std::move(request));
			}
		);
	}

	template <typename T>
	void run_callbacks(handler_t<T>& handler)
	{
		handler.requests.insert(handler.requests.end(),
			std::move_iterator(handler.new_requests.begin()), std::move_iterator(handler.new_requests.end()));
		handler.new_requests.clear();

		for (auto i = handler.requests.begin(); i != handler.requests.end(); )
		{
			if (i->result.valid() && i->result.wait_for(0s) == std::future_status::ready)
			{
				try
				{
					i->callback(i->result.get());
				}
				catch (const std::exception& e)
				{
					console::error("LUI: error executing http request callback: %s\n", e.what());
				}

				i = handler.requests.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	void run_frame(sol::state& state)
	{
		run_callbacks(http_handler);
		run_callbacks(server_handler);
	}

	void clear()
	{
		http_handler = {};
		server_handler = {};
	}
}
