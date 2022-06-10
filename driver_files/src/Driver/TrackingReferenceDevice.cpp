#include "TrackingReferenceDevice.hpp"

ExampleDriver::TrackingReferenceDevice::TrackingReferenceDevice(std::string serial):
    serial_(serial)
{

    // Get some random angle to place this tracking reference at in the scene
    //this->random_angle_rad_ = fmod(rand() / 10000.f, 2 * 3.14159f);
}

std::string ExampleDriver::TrackingReferenceDevice::GetSerial()
{
    return this->serial_;
}

void ExampleDriver::TrackingReferenceDevice::Update()
{
    if (this->device_index_ == vr::k_unTrackedDeviceIndexInvalid)
        return;


    // Setup pose for this frame
    auto pose = IVRDevice::MakeDefaultPose();

    linalg::vec<float, 3> device_position{ 0.f, 1.f, 1.f };

    linalg::vec<float, 4> y_quat{ 0, std::sinf(this->random_angle_rad_ / 2), 0, std::cosf(this->random_angle_rad_ / 2) }; // Point inwards (z- is forward)

    linalg::vec<float, 4> x_look_down{ std::sinf((-3.1415f/4) / 2), 0, 0, std::cosf((-3.1415f / 4) / 2) }; // Tilt downwards to look at the centre

    linalg::vec<float, 4> device_rotation = linalg::qmul(y_quat, x_look_down);

    device_position = linalg::qrot(y_quat, device_position);

    pose.vecPosition[0] = device_position.x;
    pose.vecPosition[1] = device_position.y;
    pose.vecPosition[2] = device_position.z;

    pose.qRotation.w = device_rotation.w;
    pose.qRotation.x = device_rotation.x;
    pose.qRotation.y = device_rotation.y;
    pose.qRotation.z = device_rotation.z;

    // Post pose
    GetDriver()->GetDriverHost()->TrackedDevicePoseUpdated(this->device_index_, pose, sizeof(vr::DriverPose_t));
    this->last_pose_ = pose;
}

void ExampleDriver::TrackingReferenceDevice::UpdatePose(double a, double b, double c, double qw, double qx, double qy, double qz)
{
    // Setup pose for this frame
    auto pose = IVRDevice::MakeDefaultPose();

    pose.vecPosition[0] = a;
    pose.vecPosition[1] = b;
    pose.vecPosition[2] = c;

    pose.qRotation.w = qw;
    pose.qRotation.x = qx;
    pose.qRotation.y = qy;
    pose.qRotation.z = qz;

    // Post pose
    GetDriver()->GetDriverHost()->TrackedDevicePoseUpdated(this->device_index_, pose, sizeof(vr::DriverPose_t));
    this->last_pose_ = pose;
}

DeviceType ExampleDriver::TrackingReferenceDevice::GetDeviceType()
{
    return DeviceType::TRACKING_REFERENCE;
}

vr::TrackedDeviceIndex_t ExampleDriver::TrackingReferenceDevice::GetDeviceIndex()
{
    return this->device_index_;
}

vr::EVRInitError ExampleDriver::TrackingReferenceDevice::Activate(uint32_t unObjectId)
{
    this->device_index_ = unObjectId;

    GetDriver()->Log("Activating tracking reference " + this->serial_);

    // Get the properties handle
    auto props = GetDriver()->GetProperties()->TrackedDeviceToPropertyContainer(this->device_index_);

    // Set some universe ID (Must be 2 or higher)
    GetDriver()->GetProperties()->SetUint64Property(props, vr::Prop_CurrentUniverseId_Uint64, 2);
    
    // Set up a model "number" (not needed but good to have)
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, "apriltag_trackingreference");

    // Set up a render model path
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_RenderModelName_String, "dk2_camera");

    // Set the icons
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReady_String, "{apriltagtrackers}/icons/trackingreference_ready.png");

    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceOff_String, "{apriltagtrackers}/icons/trackingreference_not_ready.png");
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearching_String, "{apriltagtrackers}/icons/trackingreference_not_ready.png");
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{apriltagtrackers}/icons/trackingreference_not_ready.png");
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{apriltagtrackers}/icons/trackingreference_not_ready.png");
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceNotReady_String, "{apriltagtrackers}/icons/trackingreference_not_ready.png");
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandby_String, "{apriltagtrackers}/icons/trackingreference_not_ready.png");
    GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceAlertLow_String, "{apriltagtrackers}/icons/trackingreference_not_ready.png");

    return vr::EVRInitError::VRInitError_None;
}

void ExampleDriver::TrackingReferenceDevice::Deactivate()
{
    this->device_index_ = vr::k_unTrackedDeviceIndexInvalid;
}

void ExampleDriver::TrackingReferenceDevice::EnterStandby()
{
}

void* ExampleDriver::TrackingReferenceDevice::GetComponent(const char* pchComponentNameAndVersion)
{
    return nullptr;
}

void ExampleDriver::TrackingReferenceDevice::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
    if (unResponseBufferSize >= 1)
        pchResponseBuffer[0] = 0;
}

vr::DriverPose_t ExampleDriver::TrackingReferenceDevice::GetPose()
{
    return last_pose_;
}
