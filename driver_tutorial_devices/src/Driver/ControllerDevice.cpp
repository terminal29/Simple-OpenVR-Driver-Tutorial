#include "ControllerDevice.hpp"
#include <string>
#include <Windows.h>

TutorialDriver::ControllerDevice::ControllerDevice(std::string serial, bool isLeftController):m_serial(serial),m_isLeft(isLeftController)
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






    // Post pose
    auto pose = this->GetPose();
    pose.vecPosition[0] = this->m_isLeft ? 1 : -1;
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
    
    // Set up a model "number" (not needed but good to have)
    vr::VRProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, "Tutorial Controller Device");

    // Set render model which is visible in SteamVR and some SteamVR apps
    vr::VRProperties()->SetStringProperty(props, vr::Prop_RenderModelName_String, "locator");

    if (this->m_isLeft) {
        vr::VRProperties()->SetInt32Property(props, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_LeftHand);
    } else {
        vr::VRProperties()->SetInt32Property(props, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_RightHand);
    }

    // Set up icon paths
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReady_String, "{tutorial_devices}/icons/controller_ready.png");

    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceOff_String, "{tutorial_devices}/icons/controller_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearching_String, "{tutorial_devices}/icons/controller_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{tutorial_devices}/icons/controller_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{tutorial_devices}/icons/controller_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceNotReady_String, "{tutorial_devices}/icons/controller_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandby_String, "{tutorial_devices}/icons/controller_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceAlertLow_String, "{tutorial_devices}/icons/controller_not_ready.png");

    // Create input/output paths
    vr::VRDriverInput()->CreateHapticComponent(props, "/output/haptic", &this->m_compHaptic);

    vr::VRDriverInput()->CreateBooleanComponent(props, "/input/a/touch", &this->m_compATouch);
    vr::VRDriverInput()->CreateBooleanComponent(props, "/input/a/click", &this->m_compAClick);
    
    vr::VRDriverInput()->CreateBooleanComponent(props, "/input/b/touch", &this->m_compBTouch);
    vr::VRDriverInput()->CreateBooleanComponent(props, "/input/b/click", &this->m_compBClick);
    
    vr::VRDriverInput()->CreateBooleanComponent(props, "/input/system/touch", &this->m_compSysTouch);
    vr::VRDriverInput()->CreateBooleanComponent(props, "/input/system/click", &this->m_compSysClick);

    vr::VRDriverInput()->CreateScalarComponent(props, "/input/trigger/value", &this->m_compTriggerValue, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedOneSided);
    vr::VRDriverInput()->CreateBooleanComponent(props, "/input/trigger/touch", &this->m_compTriggerTouch);
    vr::VRDriverInput()->CreateBooleanComponent(props, "/input/trigger/click", &this->m_compTriggerClick);

    vr::VRDriverInput()->CreateScalarComponent(props, "/input/grip/value", &this->m_compGripValue, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedOneSided);
    vr::VRDriverInput()->CreateScalarComponent(props, "/input/grip/force", &this->m_compGripForce, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedOneSided);
    vr::VRDriverInput()->CreateBooleanComponent(props, "/input/grip/touch", &this->m_compGripTouch);
    vr::VRDriverInput()->CreateBooleanComponent(props, "/input/grip/click", &this->m_compGripClick);

    vr::VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/x", &this->m_compTrackpadX, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);
    vr::VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/y", &this->m_compTrackpadY, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);
    vr::VRDriverInput()->CreateBooleanComponent(props, "/input/trackpad/touch", &this->m_compTrackpadTouch);
    vr::VRDriverInput()->CreateBooleanComponent(props, "/input/trackpad/click", &this->m_compTrackpadClick);

    vr::VRDriverInput()->CreateScalarComponent(props, "/input/joystick/x", &this->m_compJoystickX, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);
    vr::VRDriverInput()->CreateScalarComponent(props, "/input/joystick/y", &this->m_compJoystickY, vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);
    vr::VRDriverInput()->CreateBooleanComponent(props, "/input/joystick/touch", &this->m_compJoystickTouch);
    vr::VRDriverInput()->CreateBooleanComponent(props, "/input/joystick/click", &this->m_compJoystickClick);


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