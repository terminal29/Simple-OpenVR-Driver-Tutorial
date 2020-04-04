#include "ControllerDevice.hpp"
#include <sstream>

ExampleDriver::ControllerDevice::ControllerDevice(std::string serial, ControllerDevice::Handedness handedness):
    serial_(serial),
    handedness_(handedness)
{
}

std::string ExampleDriver::ControllerDevice::GetSerial()
{
    return this->serial_;
}

void ExampleDriver::ControllerDevice::Update()
{
    if (this->device_index_ == vr::k_unTrackedDeviceIndexInvalid)
        return;

    // Check if this device was asked to be identified
    auto events = GetDriver()->GetOpenVREvents();
    for (auto event : events) {
        // Note here, event.trackedDeviceIndex does not necissarily equal this->device_index_, not sure why, but the component handle will match so we can just use that instead
        //if (event.trackedDeviceIndex == this->device_index_) {
        if (event.eventType == vr::EVREventType::VREvent_Input_HapticVibration) {
            if (event.data.hapticVibration.componentHandle == this->haptic_component_) {
                this->did_vibrate_ = true;
            }
        }
        //}
    }

    // Check if we need to keep vibrating
    if (this->did_vibrate_) {
        this->vibrate_anim_state_ += (GetDriver()->GetLastFrameTime().count()/1000.f);
        if (this->vibrate_anim_state_ > 1.0f) {
            this->did_vibrate_ = false;
            this->vibrate_anim_state_ = 0.0f;
        }
    }

    // Setup pose for this frame
    auto pose = IVRDevice::MakeDefaultPose();

    // Find a HMD
    auto devices = GetDriver()->GetDevices();
    auto hmd = std::find_if(devices.begin(), devices.end(), [](const std::shared_ptr<IVRDevice>& device_ptr) {return device_ptr->GetDeviceType() == DeviceType::HMD; });
    if (hmd != devices.end()) {
        // Found a HMD
        vr::DriverPose_t hmd_pose = (*hmd)->GetPose();

        // Here we setup some transforms so our controllers are offset from the headset by a small amount so we can see them
        linalg::vec<float, 3> hmd_position{ (float)hmd_pose.vecPosition[0], (float)hmd_pose.vecPosition[1], (float)hmd_pose.vecPosition[2] };
        linalg::vec<float, 4> hmd_rotation{ (float)hmd_pose.qRotation.x, (float)hmd_pose.qRotation.y, (float)hmd_pose.qRotation.z, (float)hmd_pose.qRotation.w };

        // Do shaking animation if haptic vibration was requested
        float controller_y = -0.2f + 0.01f * std::sinf(8 * 3.1415f * vibrate_anim_state_);

        // Left hand controller on the left, right hand controller on the right, any other handedness sticks to the middle
        float controller_x = this->handedness_ == Handedness::LEFT ? -0.2f : (this->handedness_ == Handedness::RIGHT ? 0.2f : 0.f);

        linalg::vec<float, 3> hmd_pose_offset = { controller_x, controller_y, -0.5f };

        hmd_pose_offset = linalg::qrot(hmd_rotation, hmd_pose_offset);

        linalg::vec<float, 3> final_pose = hmd_pose_offset + hmd_position;

        pose.vecPosition[0] = final_pose.x;
        pose.vecPosition[1] = final_pose.y;
        pose.vecPosition[2] = final_pose.z;

        pose.qRotation.w = hmd_rotation.w;
        pose.qRotation.x = hmd_rotation.x;
        pose.qRotation.y = hmd_rotation.y;
        pose.qRotation.z = hmd_rotation.z;
    }

    // Post pose
    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(this->device_index_, pose, sizeof(vr::DriverPose_t));
    this->last_pose_ = pose;
}

DeviceType ExampleDriver::ControllerDevice::GetDeviceType()
{
    return DeviceType::CONTROLLER;
}

ExampleDriver::ControllerDevice::Handedness ExampleDriver::ControllerDevice::GetHandedness()
{
    return this->handedness_;
}

vr::TrackedDeviceIndex_t ExampleDriver::ControllerDevice::GetDeviceIndex()
{
    return this->device_index_;
}

vr::EVRInitError ExampleDriver::ControllerDevice::Activate(uint32_t unObjectId)
{
    this->device_index_ = unObjectId;

    // Get the properties handle
    auto props = GetDriver()->GetProperties()->TrackedDeviceToPropertyContainer(this->device_index_);

    // Setup inputs and outputs
    GetDriver()->GetInput()->CreateHapticComponent(props, "/output/haptic", &this->haptic_component_);

    // Set some universe ID (Must be 2 or higher)
    GetDriver()->GetProperties()->SetUint64Property(props, vr::Prop_CurrentUniverseId_Uint64, 2);
    
    // Set up a model "number" (not needed but good to have)
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, "example_controller");

    // Set controller profile
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_InputProfilePath_String, "{example}/input/example_controller_bindings.json");

    // Change the icon depending on which handedness this controller is using (ANY uses right)
    std::string controller_handedness_str = this->handedness_ == Handedness::LEFT ? "left" : "right";
    std::string controller_ready_file = "{example}/icons/controller_ready_" + controller_handedness_str + ".png";
    std::string controller_not_ready_file = "{example}/icons/controller_not_ready_" + controller_handedness_str + ".png";

    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReady_String, controller_ready_file.c_str());

    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceOff_String, controller_not_ready_file.c_str());
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearching_String, controller_not_ready_file.c_str());
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearchingAlert_String, controller_not_ready_file.c_str());
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReadyAlert_String, controller_not_ready_file.c_str());
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceNotReady_String, controller_not_ready_file.c_str());
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandby_String, controller_not_ready_file.c_str());
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceAlertLow_String, controller_not_ready_file.c_str());

    return vr::EVRInitError::VRInitError_None;
}

void ExampleDriver::ControllerDevice::Deactivate()
{
    this->device_index_ = vr::k_unTrackedDeviceIndexInvalid;
}

void ExampleDriver::ControllerDevice::EnterStandby()
{
}

void* ExampleDriver::ControllerDevice::GetComponent(const char* pchComponentNameAndVersion)
{
    return nullptr;
}

void ExampleDriver::ControllerDevice::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
    if (unResponseBufferSize >= 1)
        pchResponseBuffer[0] = 0;
}

vr::DriverPose_t ExampleDriver::ControllerDevice::GetPose()
{
    return last_pose_;
}
