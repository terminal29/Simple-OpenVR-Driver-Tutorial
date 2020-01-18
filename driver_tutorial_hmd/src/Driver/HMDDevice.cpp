#include "HMDDevice.hpp"
#include <Windows.h>

TutorialDriver::HMDDevice::HMDDevice(std::string serial):m_serial(serial)
{
}

std::string TutorialDriver::HMDDevice::serial()
{
    return this->m_serial;
}

void TutorialDriver::HMDDevice::update(std::vector<vr::VREvent_t> events)
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
    this->m_yRot += (1.0 * (GetAsyncKeyState(VK_RIGHT) == 0) - 1.0 * (GetAsyncKeyState(VK_LEFT) == 0)) * deltaTimeSeconds;
    this->m_xRot += (-1.0 * (GetAsyncKeyState(VK_UP) == 0) + 1.0 * (GetAsyncKeyState(VK_DOWN) == 0)) * deltaTimeSeconds;
    this->m_xRot = std::fmax(this->m_xRot, -3.14159/2);
    this->m_xRot = std::fmin(this->m_xRot, 3.14159/2);

    linalg::vec<float, 4> yQuat{ 0, std::sinf(this->m_yRot / 2), 0, std::cosf(this->m_yRot / 2) };

    linalg::vec<float, 4> xQuat{ std::sinf(this->m_xRot / 2), 0, 0, std::cosf(this->m_xRot / 2) };

    linalg::vec<float, 4> pose_rot = linalg::qmul(yQuat, xQuat);

    pose.qRotation.w = pose_rot.w;
    pose.qRotation.x = pose_rot.x;
    pose.qRotation.y = pose_rot.y;
    pose.qRotation.z = pose_rot.z;

    linalg::vec<float, 3> forward_vec{-1.0f * (GetAsyncKeyState(0x44) == 0) + 1.0f * (GetAsyncKeyState(0x41) == 0), 0, 0};
    
    linalg::vec<float, 3> right_vec{0, 0, 1.0f * (GetAsyncKeyState(0x57) == 0) - 1.0f * (GetAsyncKeyState(0x53) == 0) };

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


    vr::VRProperties()->SetFloatProperty(props, vr::Prop_SecondsFromVsyncToPhotons_Float, 0.f);

    vr::VRProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, "Tutorial HMD Device");

    vr::VRProperties()->SetStringProperty(props, vr::Prop_RenderModelName_String, "tutorial_hmd_model");

    vr::VRProperties()->SetFloatProperty(props, vr::Prop_UserHeadToEyeDepthMeters_Float, 0.f);

    this->m_lastFrameTime = std::chrono::system_clock::now();

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
    if (!stricmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version)) {
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
    *pnX = this->m_windowX;
    *pnY = this->m_windowY;
    *pnWidth = this->m_windowWidth;
    *pnHeight = this->m_windowHeight;
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
    *pnWidth = this->m_windowWidth;
    *pnHeight = this->m_windowHeight;
}

void TutorialDriver::HMDDevice::GetEyeOutputViewport(vr::EVREye eEye, uint32_t* pnX, uint32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight)
{
    *pnY = 0;
    *pnWidth = this->m_windowWidth / 2;
    *pnHeight = this->m_windowHeight;

    if (eEye == vr::EVREye::Eye_Left) {
        *pnX = 0;
    }
    else {
        *pnX = this->m_windowWidth / 2;
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

vr::HmdQuaternion_t TutorialDriver::HMDDevice::quat_multiply(vr::HmdQuaternion_t q1, vr::HmdQuaternion_t q2)
{
    vr::HmdQuaternion_t q_out;
    q_out.x = q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;
    q_out.y = -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y;
    q_out.z = q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;
    q_out.w = -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;
    return q_out;
}

vr::HmdVector3_t TutorialDriver::HMDDevice::quat_vec_multiply(vr::HmdQuaternion_t q, vr::HmdVector3_t v)
{
    vr::HmdQuaternion_t qVec = { 0 };
    qVec.x = v.v[0];
    qVec.y = v.v[1];
    qVec.z = v.v[2];
    qVec = this->quat_multiply(q, qVec);
    vr::HmdVector3_t vec;
    vec.v[0] = qVec.x;
    vec.v[1] = qVec.y;
    vec.v[2] = qVec.z;
    return vec;
}
