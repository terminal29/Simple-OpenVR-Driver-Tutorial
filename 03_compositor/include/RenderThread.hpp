#pragma once

#include <thread>
#include <array>
#include <condition_variable>
#include <mutex>
#include <future>

#include <openvr_driver.h>
#include <DriverLog.hpp>
#include <Rendering.hpp>

typedef std::function<void(ID3D11Device* a, ID3D11DeviceContext* b)> RenderJob;

class RenderThread {
public:
	RenderThread();
	~RenderThread();

	// starts render thread
	std::future<bool> start(std::string window_name, int width, int height, bool wait_for_completion  = false);

	void process();

	// stops render thead
	void stop(bool wait_for_completion = false);

	// Submits a render job to be run on next frame
	std::future<void> run_job(RenderJob render_job, bool wait_for_completion = false);

	// Submits a texture to be drawn on the next frame
	void draw_texture(const vr::PresentInfo_t* present_info, int width, int height, bool wait_for_completion = false);

	std::vector<RenderJob>& get_render_jobs();

	bool get_render_thread_state();

private:
	std::thread	_internal_thread;
	bool _render_thread_running;

	std::mutex _render_task_lock;
	std::vector<RenderJob> _render_tasks;

};