#include "HMDDevice.hpp"

TutorialDriver::HMDDevice::HMDDevice(std::string serial):m_serial(serial)
{
}

std::string TutorialDriver::HMDDevice::serial()
{
    return this->m_serial;
}

void TutorialDriver::HMDDevice::update(std::vector<vr::VREvent_t> events)
{
    if (this->m_deviceIndex != vr::k_unTrackedDeviceIndexInvalid)
        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(this->device_index(), this->GetPose(), sizeof(vr::TrackedDevicePose_t));
}

vr::TrackedDeviceIndex_t TutorialDriver::HMDDevice::device_index()
{
    return this->m_deviceIndex;
}

vr::EVRInitError TutorialDriver::HMDDevice::Activate(uint32_t unObjectId)
{
    this->m_deviceIndex = unObjectId;

    // Get the properties handle
    auto props = vr::VRProperties()->TrackedDeviceToPropertyContainer(this->m_deviceIndex);

    // Set some universe ID (Must be 2 or higher)
    vr::VRProperties()->SetUint64Property(props, vr::Prop_CurrentUniverseId_Uint64, 2);

    // Set the IPD to be whatever steam has configured
    vr::VRProperties()->SetFloatProperty(props, vr::Prop_UserIpdMeters_Float, vr::VRSettings()->GetFloat(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_IPD_Float));

    // Set the display FPS
    vr::VRProperties()->SetFloatProperty(props, vr::Prop_DisplayFrequency_Float, 90.f);

    // Disable warnings about compositor not being fullscreen
    vr::VRProperties()->SetBoolProperty(props, vr::Prop_IsOnDesktop_Bool, true);


    return vr::EVRInitError::VRInitError_None;
}

void TutorialDriver::HMDDevice::Deactivate()
{
    this->m_deviceIndex = vr::k_unTrackedDeviceIndexInvalid;
}

void TutorialDriver::HMDDevice::EnterStandby()
{
}

void* TutorialDriver::HMDDevice::GetComponent(const char* pchComponentNameAndVersion)
{
    if (std::strcmp(vr::IVRDisplayComponent_Version, pchComponentNameAndVersion) == 0) {
        return static_cast<vr::IVRDisplayComponent*>(this);
    }
    return nullptr;
}

void TutorialDriver::HMDDevice::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
    if (unResponseBufferSize >= 1)
        pchResponseBuffer[0] = 0;
}

vr::DriverPose_t TutorialDriver::HMDDevice::GetPose()
{
    vr::DriverPose_t out_pose = { 0 };

    out_pose.deviceIsConnected = true;
    out_pose.poseIsValid = true;
    out_pose.result = vr::ETrackingResult::TrackingResult_Running_OK;
    out_pose.willDriftInYaw = false;
    out_pose.shouldApplyHeadModel = false;
    out_pose.qDriverFromHeadRotation.w = out_pose.qWorldFromDriverRotation.w = out_pose.qRotation.w = 1.0;

    return out_pose;
}

void TutorialDriver::HMDDevice::GetWindowBounds(int32_t* pnX, int32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight)
{
    *pnX = 0;
    *pnY = 0;
    *pnWidth = 640;
    *pnHeight = 480;
}

bool TutorialDriver::HMDDevice::IsDisplayOnDesktop()
{
    return true;
}

bool TutorialDriver::HMDDevice::IsDisplayRealDisplay()
{
    return false;
}

void TutorialDriver::HMDDevice::GetRecommendedRenderTargetSize(uint32_t* pnWidth, uint32_t* pnHeight)
{
    *pnWidth = 640;
    *pnHeight = 480;
}

void TutorialDriver::HMDDevice::GetEyeOutputViewport(vr::EVREye eEye, uint32_t* pnX, uint32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight)
{
    *pnY = 0;
    *pnWidth = 640 / 2;
    *pnHeight =480;

    if (eEye == vr::EVREye::Eye_Left) {
        *pnX = 640;
    }
    else {
        *pnX = 640 / 2;
    }
}

void TutorialDriver::HMDDevice::GetProjectionRaw(vr::EVREye eEye, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom)
{
    *pfLeft = -1;
    *pfRight = 1;
    *pfTop = -1;
    *pfBottom = 1;
}

vr::DistortionCoordinates_t TutorialDriver::HMDDevice::ComputeDistortion(vr::EVREye eEye, float fU, float fV)
{
    vr::DistortionCoordinates_t coordinates;
    coordinates.rfBlue[0] = fU;
    coordinates.rfBlue[1] = fV;
    coordinates.rfGreen[0] = fU;
    coordinates.rfGreen[1] = fV;
    coordinates.rfRed[0] = fU;
    coordinates.rfRed[1] = fV;
    return coordinates;
}
