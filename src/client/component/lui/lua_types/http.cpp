#include <std_include.hpp>

#include "http.hpp"
#include "../../console.hpp"

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

		std::deque<http_request_t> requests;
	}

	void setup(sol::state& state)
	{
		state["http"] = state.create_table();

		state["http"]["get"] = [](const std::string& url, const sol::unsafe_function& callback)
		{
			http_request_t request{};
			request.callback = callback;
			request.result = utils::http::get_data_async(url);
			requests.emplace_back(std::move(request));
		};
	}

	void run_frame(sol::state& state)
	{
		for (auto i = requests.begin(); i != requests.end(); )
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

				i = requests.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	void clear()
	{
		requests.clear();
	}
}
