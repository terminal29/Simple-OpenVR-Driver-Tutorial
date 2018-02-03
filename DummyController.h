#pragma once

#include <openvr_driver.h>
using namespace vr;

class DummyController : public ITrackedDeviceServerDriver, public IVRControllerComponent
{
public:
	DummyController();

	DummyController(std::string serial, bool side, DriverPose_t initial_pose, VRControllerState_t initial_state);

	virtual ~DummyController();

	virtual void updateControllerState(VRControllerState_t new_state);

	virtual void updateControllerPose(DriverPose_t new_pose);

	virtual uint32_t getObjectID();

	/*
		Inherited from ITrackedDeviceServerDriver:
	*/

	EVRInitError Activate(uint32_t unObjectId) override;

	void Deactivate() override;

	void EnterStandby() override;

	void *GetComponent(const char* pchComponentNameAndVersion) override;

	void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) override;

	DriverPose_t GetPose() override;

	/* 
		Inherited from IVRControllerComponent:
	*/

	VRControllerState_t GetControllerState() override;

	bool TriggerHapticPulse(uint32_t unAxisId, uint16_t usPulseDurationMicroseconds) override;

private:
	VRControllerState_t controller_state_;

	DriverPose_t controller_pose_;

	uint32_t object_id_;

	std::string serial_;

	bool side_;
};