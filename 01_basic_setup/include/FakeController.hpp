#pragma once

#include <string>
#include <chrono>

#include <openvr_driver.h>

class FakeController : public vr::ITrackedDeviceServerDriver
{
public:
	FakeController();
	virtual ~FakeController() = default;

	FakeController(FakeController&&) = default;
	FakeController& operator=(FakeController &&) = default;
	FakeController(const FakeController &) = delete;
	FakeController& operator= (const FakeController &) = delete;
		
	std::string get_serial();
	void update();
	vr::TrackedDeviceIndex_t get_index();
	void process_event(const vr::VREvent_t& event);

	// Inherited via ITrackedDeviceServerDriver
	virtual vr::EVRInitError Activate(vr::TrackedDeviceIndex_t index) override;
	virtual void Deactivate() override;
	virtual void EnterStandby() override;
	virtual void* GetComponent(const char* component) override;
	virtual void DebugRequest(const char* request, char* response_buffer, uint32_t response_buffer_size) override;
	virtual vr::DriverPose_t GetPose() override;

private:
	vr::TrackedDeviceIndex_t _index;
	vr::DriverPose_t _pose;
	vr::PropertyContainerHandle_t _props;

	struct Components {
		vr::VRInputComponentHandle_t
			_system_click,
			_grip_click,
			_app_click,
			_trigger_click,
			_trigger_value,
			_trackpad_x,
			_trackpad_y,
			_trackpad_click,
			_trackpad_touch,
			_haptic;
	};

	Components _components;

	std::string _serial;
};