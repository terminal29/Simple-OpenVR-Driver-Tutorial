#include <RenderThread.hpp>

RenderThread::RenderThread() {
	
}

void RenderThread::draw_texture(const vr::PresentInfo_t * present_info, int width, int height, bool wait_for_completion)
{
	/*run_job([&](){


	
	}, wait_for_completion);*/
}


RenderThread::~RenderThread()
{
	stop(true);
}

std::future<bool> RenderThread::start(std::string window_name, int width, int height, bool wait_for_completion) {
	std::promise<bool> completion;
	std::future<bool> completion_result = completion.get_future();
	_internal_thread = std::thread([&]{
		Rendering::set_on_render_fn([](ID3D11Device1*, ID3D11DeviceContext1*, IDXGISwapChain1*, ID3D11RenderTargetView*, ID3D11DepthStencilView*) {});
		Rendering::create_window();
		completion.set_value(true);
		while (_render_thread_running) {
			Rendering::tick();
		}
		Rendering::quit();


		/*
		// doesnt work with supplied name, width, and height values (unsure why)
		if (Rendering::initialise_window(window_name, 0, 0, width, height)) {
			completion.set_value(true);

			Rendering::set_on_render_fn([](ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext) {
				if (pUserContext == nullptr)
					return;
				RenderThread* _this = (RenderThread*)pUserContext;
				if (_this->get_render_thread_state()) {
					std::vector<RenderJob>& render_jobs = _this->get_render_jobs();
					for (const RenderJob& job : render_jobs) {
						job(pd3dDevice, pd3dImmediateContext);
					}
				}
				else {
					Rendering::close_window();
				}
			}, this);

			Rendering::main_loop();
		}
		else {
			DebugDriverLog("Error Rendering::initialise_window\n");
			completion.set_value(false);
		}
		*/
	});

	if (wait_for_completion) {
		completion_result.wait();
	}
	return completion_result;
}

void RenderThread::process() {
}

void RenderThread::stop(bool wait_for_completion) {
	_render_thread_running = false;
	//Rendering::quit();
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