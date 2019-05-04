#pragma once

#include <thread>
#include <array>
#include <condition_variable>
#include <mutex>
#include <future>

#include <openvr_driver.h>
#include <DriverLog.hpp>
#include <CompositorWindow.hpp>

typedef std::function<void(ID3D11Device* a, ID3D11DeviceContext* b)> RenderJob;

class RenderThread {
public:
	RenderThread();
	~RenderThread();

	/// <summary>
	/// Starts the render thread
	/// </summary>
	/// <param name="window_name">Window name</param>
	/// <param name="width">Window width</param>
	/// <param name="height">Window height</param>
	/// <param name="wait_for_completion">Blocks returning until the window has been created</param>
	/// <returns>A future which is resolved when the window is created</returns>
	std::future<bool> start(std::string window_name, int width, int height, bool wait_for_completion  = false);

	/// <summary>
	/// Stops the render thread
	/// </summary>
	/// <param name="wait_for_completion">Blocks returning until the window has been closed</param>
	void stop(bool wait_for_completion = false);

	/// <summary>
	/// Submits a render job to be run on next frame
	/// </summary>
	/// <param name="render_job">the render job</param>
	/// <param name="wait_for_completion">Block until completion</param>
	/// <returns>A future which is resolved when the job is completed</returns>
	std::future<void> run_job(RenderJob render_job, bool wait_for_completion = false);

	/// <summary>
	/// Submits a shared texture to be drawn on the next frame
	/// </summary>
	/// <param name="present_info">Shared texture info</param>
	/// <param name="width">Texture width</param>
	/// <param name="height">Texture height</param>
	/// <param name="wait_for_completion">Block until completion</param>
	void draw_texture(const vr::PresentInfo_t* present_info, int width, int height, bool wait_for_completion = false);

	/// <summary>
	/// Gets the current render jobs
	/// </summary>
	/// <returns></returns>
	std::vector<RenderJob>& get_render_jobs();

	/// <summary>
	/// Gets the current render thread running state
	/// </summary>
	/// <returns>current render thread running state</returns>
	bool get_render_thread_state();

private:
	std::thread	_internal_thread;
	bool _render_thread_running;

	std::mutex _render_task_lock;
	std::vector<RenderJob> _render_tasks;


};