#pragma once

#include <memory>
#include <string>
#include <chrono>

#include <openvr_driver.h>

class FakeHMD : public vr::ITrackedDeviceServerDriver, public vr::IVRDisplayComponent {
public:

	/// <summary>
	/// Makes a new instance
	/// </summary>
	/// <returns>A new FakeHMD</returns>
	static std::shared_ptr<FakeHMD> make_new();
	virtual ~FakeHMD() = default;

	/// <summary>
	/// Disable move and copy
	/// Because we give the pointer to this to VRServerDriverHost, we dont want it to ever change
	/// </summary>
	FakeHMD(FakeHMD&&) = delete;
	FakeHMD& operator=(FakeHMD &&) = delete;
	FakeHMD(const FakeHMD &) = delete;
	FakeHMD& operator= (const FakeHMD &) = delete;

	/// <summary>
	/// Gets this device's serial string
	/// </summary>
	/// <returns>Serial string</returns>
	std::string get_serial() const;

	/// <summary>
	/// Updates the internal state of this device, to be called every time ServerDriver::RunFrame is called
	/// </summary>
	void update();

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

	/// <summary>
	/// Gets the bounds for the window
	/// </summary>
	/// <param name="x">Top left x pixel</param>
	/// <param name="y">Top left y pixel</param>
	/// <param name="width">Window pixel width</param>
	/// <param name="height">Window pixel height</param>
	virtual void GetWindowBounds(int32_t* x, int32_t* y, uint32_t* width, uint32_t* height) override;
	
	/// <summary>
	/// Returns true if the display is on the desktop
	/// </summary>
	/// <returns>If the display is on the desktop</returns>
	virtual bool IsDisplayOnDesktop() override;

	/// <summary>
	/// Returns true if the display is a real display
	/// </summary>
	/// <returns>If the display is a real display</returns>
	virtual bool IsDisplayRealDisplay() override;

	/// <summary>
	/// Gets the render target size
	/// Does not have to be the same as the window bounds
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	virtual void GetRecommendedRenderTargetSize(uint32_t * width, uint32_t * height) override;
	
	/// <summary>
	/// Gets the viewport for each eye
	/// </summary>
	/// <param name="eye">Eye side</param>
	/// <param name="x">Top left x pixel</param>
	/// <param name="y">Top left y pixel</param>
	/// <param name="width">Pixel width</param>
	/// <param name="height">Pixel height</param>
	virtual void GetEyeOutputViewport(vr::EVREye eye, uint32_t * x, uint32_t * y, uint32_t * width, uint32_t * height) override;

	/// <summary>
	/// Gets the viewport size for each eye
	/// </summary>
	/// <param name="eye">Eye side</param>
	/// <param name="left">Left viewport offset</param>
	/// <param name="right">Right viewport offset</param>
	/// <param name="top">Top viewport offset</param>
	/// <param name="bottom">Bottom viewport offset</param>
	virtual void GetProjectionRaw(vr::EVREye eye, float * left, float * right, float * top, float * bottom) override;

	/// <summary>
	/// Gets the viewport distortion
	/// </summary>
	/// <param name="eye">Eye side</param>
	/// <param name="u">x pixel</param>
	/// <param name="v">y pixel</param>
	/// <returns>Distortion for the requested pixel</returns>
	virtual vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eye, float u, float v) override;

	/// <summary>
	/// Gets the current device pose
	/// </summary>
	/// <returns>Device Pose</returns>
	virtual vr::DriverPose_t get_pose() const;

	/// <summary>
	/// Sets the current device pose
	/// Note: Be sure to zero initialize the pose struct if you have created a new one
	/// </summary>
	/// <param name="new_pose">New device pose</param>
	virtual void set_pose(vr::DriverPose_t new_pose);

private:
	// Private constructor so the only way to instantiate the class is via the make_new function.
	FakeHMD();

	// Stores the openvr supplied device index.
	vr::TrackedDeviceIndex_t _index;

	// Stores the devices current pose.
	vr::DriverPose_t _pose;

	// Stores the timestamp of the pose.
	std::chrono::milliseconds _pose_timestamp;

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

	// Stores the serial for this device. Must be unique.
	std::string _serial;
};