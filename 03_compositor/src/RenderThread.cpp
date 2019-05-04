#include <RenderThread.hpp>

RenderThread::RenderThread() {
	
}

void RenderThread::draw_texture(const vr::PresentInfo_t * present_info, int width, int height, bool wait_for_completion)
{
	// TODO
}


RenderThread::~RenderThread()
{
	stop(true);
}

std::future<bool> RenderThread::start(std::string window_name, int width, int height, bool wait_for_completion) {
	std::promise<bool> completion;
	std::future<bool> completion_result = completion.get_future();
	_internal_thread = std::thread([&]{
		CompositorWindow w;
		w.Initialize(100, 100);
		completion.set_value(true);
		while (_render_thread_running) {
			w.Tick();
		}
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
	RenderJob render_task = [&](ID3D11Device* device, ID3D11DeviceContext* context) {
		render_job(device, context);
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

std::vector<RenderJob>& RenderThread::get_render_jobs() {
	return _render_tasks;
}

bool RenderThread::get_render_thread_state() {
	return _render_thread_running;
}