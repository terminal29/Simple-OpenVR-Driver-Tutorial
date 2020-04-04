#include "ControllerDevice.hpp"

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

    // Setup pose for this frame
    auto pose = IVRDevice::MakeDefaultPose();

    // Find a HMD
    auto devices = ExampleDriver::getDriver()->GetDevices();
    auto hmd = std::find_if(devices.begin(), devices.end(), [](const std::shared_ptr<IVRDevice>& device_ptr) {return device_ptr->GetDeviceType() == DeviceType::HMD; });
    if (hmd != devices.end()) {
        // Found a HMD
        vr::DriverPose_t hmd_pose = (*hmd)->GetPose();

        // Here we setup some transforms so our controllers are offset from the headset by a small amount so we can see them
        linalg::vec<float, 3> hmd_position{ (float)hmd_pose.vecPosition[0], (float)hmd_pose.vecPosition[1], (float)hmd_pose.vecPosition[2] };
        linalg::vec<float, 4> hmd_rotation{ (float)hmd_pose.qRotation.x, (float)hmd_pose.qRotation.y, (float)hmd_pose.qRotation.z, (float)hmd_pose.qRotation.w };

        // Left hand controller on the left, right hand controller on the right, any other handedness sticks to the middle
        linalg::vec<float, 3> hmd_pose_offset = { this->handedness_ == Handedness::LEFT ? -0.2f : (this->handedness_ == Handedness::RIGHT ? 0.2f : 0.f), -0.2f, -0.5f };

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
    auto props = vr::VRProperties()->TrackedDeviceToPropertyContainer(this->device_index_);

    // Set some universe ID (Must be 2 or higher)
    vr::VRProperties()->SetUint64Property(props, vr::Prop_CurrentUniverseId_Uint64, 2);
    
    // Set up a model "number" (not needed but good to have)
    vr::VRProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, "EXAMPLE_CONTROLLER_DEVICE");

    // Set up icon paths
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReady_String, "{example}/icons/controller_ready.png");

    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceOff_String, "{example}/icons/controller_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearching_String, "{example}/icons/controller_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{example}/icons/controller_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{example}/icons/controller_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceNotReady_String, "{example}/icons/controller_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandby_String, "{example}/icons/controller_not_ready.png");
    vr::VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceAlertLow_String, "{example}/icons/controller_not_ready.png");

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
