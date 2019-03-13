#pragma once

#include <thread>
#include <array>
#include <condition_variable>
#include <mutex>
#include <future>

#include <openvr_driver.h>
#include <DriverLog.hpp>

// Foward defines to keep dx stuff out of public namespace
struct IDXGIFactory1;
struct ID3D11Device;
struct ID3D11DeviceContext;
typedef void* HANDLE;

typedef std::function<void()> RenderJob;

class RenderThread {
public:
	RenderThread();
	~RenderThread();

	// starts render thread
	std::future<bool> start(std::string window_name, int width, int height, bool wait_for_completion  = false);

	// stops render thead
	void stop(bool wait_for_completion = false);

	// Submits a render job to be run on next frame
	std::future<void> run_job(RenderJob render_job, bool wait_for_completion = false);

	// Submits a texture to be drawn on the next frame
	void draw_texture(const vr::PresentInfo_t* present_info, int width, int height, bool wait_for_completion = false);

private:
	std::thread	_internal_thread;
	bool _render_thread_running;

	IDXGIFactory1* dxgi_factory = nullptr;
	ID3D11Device* d3d11_device = nullptr;
	ID3D11DeviceContext* d3d11_device_context = nullptr;
	HANDLE gl_handleD3D = nullptr;

	std::mutex _render_task_lock;
	std::vector<RenderJob> _render_tasks;

};