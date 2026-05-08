#pragma once

namespace scheduler
{
	extern std::chrono::steady_clock::time_point main_frame_begin;

	enum pipeline
	{
		async = 0,
		main = 1, // main thread
		net = 2, // server stuff
		session = 3, // session stuff
		count,
	};

	static const bool cond_continue = false;
	static const bool cond_end = true;

	void schedule(const std::function<bool()>& callback, pipeline type = pipeline::async,
		std::chrono::milliseconds delay = 0ms);
	void loop(const std::function<void()>& callback, pipeline type = pipeline::async,
		std::chrono::milliseconds delay = 0ms);
	void once(const std::function<void()>& callback, pipeline type = pipeline::async,
		std::chrono::milliseconds delay = 0ms);
}
