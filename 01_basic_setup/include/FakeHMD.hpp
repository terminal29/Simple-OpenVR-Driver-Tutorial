#pragma once

#include <string>
#include <chrono>

#include <openvr_driver.h>

class FakeHMD : public vr::ITrackedDeviceServerDriver, public vr::IVRDisplayComponent {
public:
	FakeHMD();
	virtual ~FakeHMD() = default;


	FakeHMD(FakeHMD&&) = default;
	FakeHMD& operator=(FakeHMD &&) = default;
	FakeHMD(const FakeHMD &) = delete;
	FakeHMD& operator= (const FakeHMD &) = delete;

	std::string get_serial();
	void update();
	vr::TrackedDeviceIndex_t get_index();
	void process_event(vr::VREvent_t event);

	// Inherited via ITrackedDeviceServerDriver
	virtual vr::EVRInitError Activate(vr::TrackedDeviceIndex_t index) override;
	virtual void Deactivate() override;
	virtual void EnterStandby() override;
	virtual void* GetComponent(const char* component) override;
	virtual void DebugRequest(const char* request, char* response_buffer, uint32_t response_buffer_size) override;
	virtual vr::DriverPose_t GetPose() override;

	// Inherited via IVRDisplayComponent
	virtual void GetWindowBounds(int32_t* x, int32_t* y, uint32_t* width, uint32_t* height) override;
	virtual bool IsDisplayOnDesktop() override;
	virtual bool IsDisplayRealDisplay() override;
	virtual void GetRecommendedRenderTargetSize(uint32_t * width, uint32_t * height) override;
	virtual void GetEyeOutputViewport(vr::EVREye eye, uint32_t * x, uint32_t * y, uint32_t * width, uint32_t * height) override;
	virtual void GetProjectionRaw(vr::EVREye eye, float * left, float * right, float * top, float * bottom) override;
	virtual vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eye, float u, float v) override;

private:
	vr::TrackedDeviceIndex_t _index;
	vr::DriverPose_t _pose;

	std::string _serial;
};