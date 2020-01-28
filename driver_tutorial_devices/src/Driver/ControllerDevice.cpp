#include "ControllerDevice.hpp"
#include <Windows.h>

TutorialDriver::ControllerDevice::ControllerDevice(std::string serial):m_serial(serial)
{
}

std::string TutorialDriver::ControllerDevice::serial()
{
    return this->m_serial;
}

void TutorialDriver::ControllerDevice::update(std::vector<vr::VREvent_t> events)
{
    if (this->m_deviceIndex == vr::k_unTrackedDeviceIndexInvalid)
        return;

    // Get deltatime
    auto now = std::chrono::system_clock::now();
    double deltaTimeSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastFrameTime).count()/1000.0;
    this->m_lastFrameTime = now;

    // Setup pose for this frame
    auto pose = this->GetPose();

    // Get orientation
    this->m_yRot += (1.0 * (GetAsyncKeyState(0x66) == 0) - 1.0 * (GetAsyncKeyState(0x64) == 0)) * deltaTimeSeconds;
    this->m_xRot += (-1.0 * (GetAsyncKeyState(0x68) == 0) + 1.0 * (GetAsyncKeyState(0x62) == 0)) * deltaTimeSeconds;
    this->m_xRot = std::fmax(this->m_xRot, -3.14159/2);
    this->m_xRot = std::fmin(this->m_xRot, 3.14159/2);

    linalg::vec<float, 4> yQuat{ 0, std::sinf((float)this->m_yRot / 2), 0, std::cosf((float)this->m_yRot / 2) };

    linalg::vec<float, 4> xQuat{ std::sinf((float)this->m_xRot / 2), 0, 0, std::cosf((float)this->m_xRot / 2) };

    linalg::vec<float, 4> pose_rot = linalg::qmul(yQuat, xQuat);

    pose.qRotation.w = pose_rot.w;
    pose.qRotation.x = pose_rot.x;
    pose.qRotation.y = pose_rot.y;
    pose.qRotation.z = pose_rot.z;

    // Update position based on rotation
    linalg::vec<float, 3> forward_vec{-1.0f * (GetAsyncKeyState(0x65) == 0) + 1.0f * (GetAsyncKeyState(0x60) == 0), 0, 0};
    linalg::vec<float, 3> right_vec{0, 0, 0};
    linalg::vec<float, 3> final_dir = forward_vec + right_vec;
    if (linalg::length(final_dir) > 0.01) {
        final_dir = linalg::normalize(final_dir) * (float)deltaTimeSeconds;
        final_dir = linalg::qrot(pose_rot, final_dir);
        this->m_x += final_dir.x;
        this->m_y += final_dir.y;
        this->m_z += final_dir.z;
    }

    pose.vecPosition[0] = this->m_x;
    pose.vecPosition[1] = this->m_y;
    pose.vecPosition[2] = this->m_z;

    // Post pose
    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(this->device_index(), pose, sizeof(vr::DriverPose_t));
}

vr::TrackedDeviceIndex_t TutorialDriver::ControllerDevice::device_index()
{
    return this->m_deviceIndex;
}

vr::EVRInitError TutorialDriver::ControllerDevice::Activate(uint32_t unObjectId)
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
    
    // Set up a model "number" (not needed but good to have)
    vr::VRProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, "Tutorial HMD Device");

    // Set up icon paths
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReady_String, "{tutorial_hmd}/icons/hmd_ready.png");

    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceOff_String, "{tutorial_hmd}/icons/hmd_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearching_String, "{tutorial_hmd}/icons/hmd_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{tutorial_hmd}/icons/hmd_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{tutorial_hmd}/icons/hmd_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceNotReady_String, "{tutorial_hmd}/icons/hmd_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandby_String, "{tutorial_hmd}/icons/hmd_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceAlertLow_String, "{tutorial_hmd}/icons/hmd_not_ready.png");
    
    this->m_lastFrameTime = std::chrono::system_clock::now();

    return vr::EVRInitError::VRInitError_None;
}

void TutorialDriver::ControllerDevice::Deactivate()
{
    this->m_deviceIndex = vr::k_unTrackedDeviceIndexInvalid;
}

void TutorialDriver::ControllerDevice::EnterStandby()
{
}

void* TutorialDriver::ControllerDevice::GetComponent(const char* pchComponentNameAndVersion)
{
    return nullptr;
}

void TutorialDriver::ControllerDevice::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
    if (unResponseBufferSize >= 1)
        pchResponseBuffer[0] = 0;
}

vr::DriverPose_t TutorialDriver::ControllerDevice::GetPose()
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