#include "DummyHMD.h"

DummyHMD::DummyHMD() 
{

}

DummyHMD::DummyHMD(std::string serial, DriverPose_t initial_pose):
	serial_(serial),
	hmd_pose_(initial_pose)
{
}

DummyHMD::~DummyHMD()
{
}

void DummyHMD::updateHMDPose(DriverPose_t new_pose)
{
	hmd_pose_ = new_pose;
}

uint32_t DummyHMD::getObjectID()
{
	return object_id_;
}

EVRInitError DummyHMD::Activate(uint32_t unObjectId)
{
	object_id_ = unObjectId;
	PropertyContainerHandle_t prop_handle = VRProperties()->TrackedDeviceToPropertyContainer(object_id_);
	VRProperties()->SetFloatProperty(prop_handle, Prop_UserIpdMeters_Float, 0.065f);
	VRProperties()->SetFloatProperty(prop_handle, Prop_UserHeadToEyeDepthMeters_Float, 0.f);
	VRProperties()->SetFloatProperty(prop_handle, Prop_DisplayFrequency_Float, 90.f);
	VRProperties()->SetFloatProperty(prop_handle, Prop_SecondsFromVsyncToPhotons_Float, 0.1f);
	VRProperties()->SetUint64Property(prop_handle, Prop_CurrentUniverseId_Uint64, 2);

	hmd_pose_.poseIsValid = true;
	hmd_pose_.result = TrackingResult_Running_OK;
	hmd_pose_.deviceIsConnected = true;

	hmd_pose_.qWorldFromDriverRotation = { 1, 0, 0, 0 };
	hmd_pose_.qDriverFromHeadRotation = { 1, 0, 0, 0 };

	return EVRInitError::VRInitError_None;
}

void DummyHMD::Deactivate()
{
}

void DummyHMD::EnterStandby()
{
}

void * DummyHMD::GetComponent(const char * pchComponentNameAndVersion)
{
	if (0 == strcmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version))
	{
		return (vr::IVRDisplayComponent*)this;
	}

	return NULL;
}

void DummyHMD::DebugRequest(const char * pchRequest, char * pchResponseBuffer, uint32_t unResponseBufferSize)
{
	if (unResponseBufferSize >= 1)
		pchResponseBuffer[0] = 0;
}

DriverPose_t DummyHMD::GetPose()
{
	return hmd_pose_;
}

void DummyHMD::GetWindowBounds(int32_t * pnX, int32_t * pnY, uint32_t * pnWidth, uint32_t * pnHeight)
{
	*pnX = window_x_;
	*pnY = window_y_;
	*pnWidth = window_width_;
	*pnHeight = window_height_;
}

bool DummyHMD::IsDisplayOnDesktop()
{
	return true;
}

bool DummyHMD::IsDisplayRealDisplay()
{
	return false;
}

void DummyHMD::GetRecommendedRenderTargetSize(uint32_t * pnWidth, uint32_t * pnHeight)
{
	*pnWidth = window_width_;
	*pnHeight = window_height_;
}

void DummyHMD::GetEyeOutputViewport(EVREye eEye, uint32_t * pnX, uint32_t * pnY, uint32_t * pnWidth, uint32_t * pnHeight)
{
	*pnY = 0;
	*pnWidth = (window_width_ / 2) - separation_;
	*pnHeight = window_height_;

	if (eEye == Eye_Left)
	{
		*pnX = separation_;
	}
	else
	{
		*pnX = window_width_ / 2;
	}
}

void DummyHMD::GetProjectionRaw(EVREye eEye, float * pfLeft, float * pfRight, float * pfTop, float * pfBottom)
{
	*pfLeft = -1.0;
	*pfRight = 1.0;
	*pfTop = -1.0;
	*pfBottom = 1.0;
}

DistortionCoordinates_t DummyHMD::ComputeDistortion(EVREye eEye, float fU, float fV)
{
	vr::DistortionCoordinates_t oDistortion{};
	oDistortion.rfBlue[0] = fU;
	oDistortion.rfBlue[1] = fV;
	oDistortion.rfGreen[0] = fU;
	oDistortion.rfGreen[1] = fV;
	oDistortion.rfRed[0] = fU;
	oDistortion.rfRed[1] = fV;
	return oDistortion;
}
