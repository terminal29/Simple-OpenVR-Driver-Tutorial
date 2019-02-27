#pragma once 

#include <openvr_driver.h>
#include <memory>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <future>

#include "DriverLog.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

class VirtualCompositor : public vr::ITrackedDeviceServerDriver, public vr::IVRVirtualDisplay, public vr::IVRDisplayComponent {
public:
	/// <summary>
		/// Makes a new instance
		/// </summary>
		/// <returns>A new FakeTracker</returns>
	static std::shared_ptr<VirtualCompositor> make_new();
	virtual ~VirtualCompositor();

	/// <summary>
	/// Disable move and copy
	/// Because we give the pointer to this to VRServerDriverHost, we dont want it to ever change
	/// </summary>
	VirtualCompositor(VirtualCompositor&&) = delete;
	VirtualCompositor& operator=(VirtualCompositor&&) = delete;
	VirtualCompositor(const VirtualCompositor&) = delete;
	VirtualCompositor& operator= (const VirtualCompositor&) = delete;

	/// <summary>
	/// Gets this device's serial string
	/// </summary>
	/// <returns>Serial string</returns>
	std::string get_serial() const;

	/// <summary>
	/// Updates the internal state of this device, to be called every time ServerDriver::RunFrame is called
	/// Override this with your custom controller functionality
	/// </summary>
	virtual void update();

	/// <summary>
	/// Gets this devices global(?) index/object id
	/// </summary>
	/// <returns>Index</returns>
	vr::TrackedDeviceIndex_t get_index() const;

	/// <summary>
	/// Processes an event
	/// </summary>
	/// <param name="event">The event to be processed</param>
	void process_event(const vr::VREvent_t& event);

	/// <summary>
	/// Activates this device
	/// Is called when vr::VRServerDriverHost()->TrackedDeviceAdded is called
	/// </summary>
	/// <param name="index">The device index</param>
	/// <returns>Error code</returns>
	virtual vr::EVRInitError Activate(vr::TrackedDeviceIndex_t index) override;

	/// <summary>
	/// Deactivates the device
	/// </summary>
	virtual void Deactivate() override;

	/// <summary>
	/// Tells the device to enter stand-by mode
	/// </summary>
	virtual void EnterStandby() override;

	/// <summary>
	/// Gets a specific component from this device
	/// </summary>
	/// <param name="component">Requested component</param>
	/// <returns>Non-owning pointer to the component</returns>
	virtual void* GetComponent(const char* component) override;

	/// <summary>
	/// Handles a debug request
	/// </summary>
	/// <param name="request">Request type</param>
	/// <param name="response_buffer">Response buffer</param>
	/// <param name="response_buffer_size">Response buffer size</param>
	virtual void DebugRequest(const char* request, char* response_buffer, uint32_t response_buffer_size) override;

	/// <summary>
	/// Gets the current device pose
	/// </summary>
	/// <returns>Device Pose</returns>
	virtual vr::DriverPose_t GetPose() override;

	// Inherited via IVRVirtualDisplay
	virtual void Present(const vr::PresentInfo_t* present_info, uint32_t present_info_size) override;

	virtual void WaitForPresent() override;

	virtual bool GetTimeSinceLastVsync(float * seconds_since_last_vsync, uint64_t * frame_counter) override;


	// Inherited via IVRDisplayComponent
	virtual void GetWindowBounds(int32_t * pnX, int32_t * pnY, uint32_t * pnWidth, uint32_t * pnHeight) override;

	virtual bool IsDisplayOnDesktop() override;

	virtual bool IsDisplayRealDisplay() override;

	virtual void GetRecommendedRenderTargetSize(uint32_t * pnWidth, uint32_t * pnHeight) override;

	virtual void GetEyeOutputViewport(vr::EVREye eEye, uint32_t * pnX, uint32_t * pnY, uint32_t * pnWidth, uint32_t * pnHeight) override;

	virtual void GetProjectionRaw(vr::EVREye eEye, float * pfLeft, float * pfRight, float * pfTop, float * pfBottom) override;

	virtual vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eEye, float fU, float fV) override;

private:
	// Private constructor so the only way to instantiate the class is via the make_new function.
	VirtualCompositor();

	void DrawTexture(const vr::PresentInfo_t* present_info);
	

	// Stores the openvr supplied device index.
	vr::TrackedDeviceIndex_t _index;

	// Stores the serial for this device. Must be unique.
	std::string _serial;

	std::thread _render_thread;

	bool _compositor_running = false;

	// An identifier for openvr for when we want to make property changes to this device.
	vr::PropertyContainerHandle_t _props;

	// A struct for concise storage of all the display properties for this device.
	struct DisplayProperties {
		int display_offset_x = 0;
		int display_offset_y = 0;
		int display_width = 1920;
		int display_height = 1080;
		int render_width = 1920;
		int render_height = 1080;
	};

	DisplayProperties _display_properties;

	std::unique_lock<std::mutex> _render_task_lock;

	std::vector<std::function<void(void)>> _render_tasks;

};