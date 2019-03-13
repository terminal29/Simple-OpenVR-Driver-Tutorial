#include <RenderThread.hpp>
#include <d3d11.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

RenderThread::RenderThread() {
	
}

// https://github.com/ValveSoftware/openvr/issues/539
// https://github.com/nlguillemot/OpenGL-on-DXGI/blob/master/main.cpp
void RenderThread::draw_texture(const vr::PresentInfo_t * present_info, int width, int height, bool wait_for_completion)
{
	run_job([&](){



	}, wait_for_completion);
}


RenderThread::~RenderThread()
{
	stop(true);
}

std::future<bool> RenderThread::start(std::string window_name, int width, int height, bool wait_for_completion) {
	std::promise<bool> completion;
	std::future<bool> completion_result = completion.get_future();
	_internal_thread = std::thread([&]() {
		
	});
	
	if (wait_for_completion) {
		completion_result.wait();
	}
	return completion_result;
}

void RenderThread::stop(bool wait_for_completion) {
	_render_thread_running = false;
	if (wait_for_completion) {
		_internal_thread.join();
	}
}

std::future<void> RenderThread::run_job(RenderJob render_job, bool wait_for_completion) {
	std::promise<void> result;
	auto render_task = [&]() {
		render_job();
		result.set_value();
	};
	std::future<void> future = result.get_future();
	{
		std::lock_guard<std::mutex> task_guard(_render_task_lock);
		_render_tasks.push_back(render_task);
	}
	if (wait_for_completion) {
		future.wait();
	}
	return future;
}
