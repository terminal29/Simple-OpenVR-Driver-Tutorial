#include "ServerDriver.h"

ServerDriver::ServerDriver()
{
}

ServerDriver::~ServerDriver()
{
}

EVRInitError ServerDriver::Init(IVRDriverContext * pDriverContext)
{
	VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

	DriverPose_t test_pose = {0};
	test_pose.deviceIsConnected = true;
	test_pose.poseIsValid = true;
	test_pose.willDriftInYaw = false;
	test_pose.shouldApplyHeadModel = false;
	test_pose.poseTimeOffset = 0;
	test_pose.result = ETrackingResult::TrackingResult_Running_OK;
	test_pose.qDriverFromHeadRotation = { 1,0,0,0 };
	test_pose.qWorldFromDriverRotation = { 1,0,0,0 };	

	VRControllerState_t test_state;
	test_state.ulButtonPressed = test_state.ulButtonTouched = 0;

	controller_left = DummyController("example_con1", false, test_pose, test_state);
	controller_right = DummyController("example_con2", true, test_pose, test_state);

	VRServerDriverHost()->TrackedDeviceAdded("example_con1", vr::TrackedDeviceClass_Controller, &controller_left);
	VRServerDriverHost()->TrackedDeviceAdded("example_con2", vr::TrackedDeviceClass_Controller, &controller_right);

	hmd = DummyHMD("dummy_hmd", test_pose);
	VRServerDriverHost()->TrackedDeviceAdded("dummy_hmd", vr::TrackedDeviceClass_HMD, &hmd);
		
	return EVRInitError::VRInitError_None;
}

void ServerDriver::Cleanup()
{
	// TODO
}

const char * const * ServerDriver::GetInterfaceVersions()
{
	return k_InterfaceVersions;
}

void ServerDriver::RunFrame()
{
	static double angle = 0;
	angle += 0.01;

	static milliseconds lastMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	milliseconds deltaTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - lastMillis;
	lastMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	{
		DriverPose_t left_pose = controller_left.GetPose();
		DriverPose_t left_previous = left_pose;

		left_pose.vecPosition[0] = 0.5 * std::sin(angle);
		left_pose.vecPosition[2] = 0.5 * -std::abs(std::cos(angle));
		left_pose.vecVelocity[0] = (left_pose.vecPosition[0] - left_previous.vecPosition[0]) * 1000 / std::max((int)deltaTime.count(), 1);
		left_pose.vecVelocity[2] = (left_pose.vecPosition[2] - left_previous.vecPosition[2]) * 1000 / std::max((int)deltaTime.count(), 1);
	
		controller_left.updateControllerPose(left_pose);
		VRServerDriverHost()->TrackedDevicePoseUpdated(controller_left.getObjectID(), controller_left.GetPose(), sizeof(DriverPose_t));
	}
	{
		DriverPose_t right_pose = controller_right.GetPose();
		DriverPose_t right_previous = right_pose;

		right_pose.vecPosition[0] = 0.5 * -std::sin(angle);
		right_pose.vecPosition[2] = 0.5 * -std::abs(std::cos(angle));
		right_pose.vecVelocity[0] = (right_pose.vecPosition[0] - right_previous.vecPosition[0]) * 1000 / std::max((int)deltaTime.count(), 1);
		right_pose.vecVelocity[2] = (right_pose.vecPosition[2] - right_previous.vecPosition[2]) * 1000 / std::max((int)deltaTime.count(), 1);
		controller_right.updateControllerPose(right_pose);
		VRServerDriverHost()->TrackedDevicePoseUpdated(controller_right.getObjectID(), controller_right.GetPose(), sizeof(DriverPose_t));
	}
	{
		DriverPose_t hmd_pose = hmd.GetPose();
		DriverPose_t hmd_previous = hmd_pose;
		hmd_pose.vecPosition[1] = 0.5 * -std::sin(angle);
		hmd_pose.vecVelocity[1] = (hmd_pose.vecPosition[1] - hmd_previous.vecPosition[1]) * 1000 / std::max((int)deltaTime.count(), 1);
		hmd.updateHMDPose(hmd_pose);
		VRServerDriverHost()->TrackedDevicePoseUpdated(hmd.getObjectID(), hmd.GetPose(), sizeof(DriverPose_t));
	}
}

bool ServerDriver::ShouldBlockStandbyMode()
{
	return false;
}

void ServerDriver::EnterStandby()
{
}

void ServerDriver::LeaveStandby()
{
}
