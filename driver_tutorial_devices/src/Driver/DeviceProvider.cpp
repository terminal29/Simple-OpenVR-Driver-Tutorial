#include "DeviceProvider.hpp"


vr::EVRInitError TutorialDriver::DeviceProvider::Init(vr::IVRDriverContext* pDriverContext)
{
    // Perform driver context initialisation
    if (vr::EVRInitError init_error = vr::InitServerDriverContext(pDriverContext); init_error != vr::EVRInitError::VRInitError_None) {
        return init_error;
    }

    auto leftControllerDevice = std::make_shared<ControllerDevice>("Tutorial_LeftControllerDevice");
    if (vr::VRServerDriverHost()->TrackedDeviceAdded(leftControllerDevice->serial().c_str(), vr::ETrackedDeviceClass::TrackedDeviceClass_Controller, leftControllerDevice.get()))
        this->m_devices.push_back(leftControllerDevice);

    auto rightControllerDevice = std::make_shared<ControllerDevice>("Tutorial_RightControllerDevice");
    if (vr::VRServerDriverHost()->TrackedDeviceAdded(rightControllerDevice->serial().c_str(), vr::ETrackedDeviceClass::TrackedDeviceClass_Controller, rightControllerDevice.get()))
        this->m_devices.push_back(rightControllerDevice);

    /*auto trackerDevice = std::make_shared<TrackerDevice>("Tutorial_TrackerDevice");
    if (vr::VRServerDriverHost()->TrackedDeviceAdded(trackerDevice->serial().c_str(), vr::ETrackedDeviceClass::TrackedDeviceClass_Controller, trackerDevice.get()))
        this->m_devices.push_back(trackerDevice);

    auto trackingReference = std::make_shared<TrackingReferenceDevice>("Tutorial_TrackingReferenceDevice");
    if (vr::VRServerDriverHost()->TrackedDeviceAdded(trackingReference->serial().c_str(), vr::ETrackedDeviceClass::TrackedDeviceClass_TrackingReference, trackingReference.get()))
        this->m_devices.push_back(trackingReference);*/
        

	return vr::VRInitError_None;
}

void TutorialDriver::DeviceProvider::Cleanup()
{
}

const char* const* TutorialDriver::DeviceProvider::GetInterfaceVersions()
{
    return vr::k_InterfaceVersions;
}

void TutorialDriver::DeviceProvider::RunFrame()
{
    // Collect events
    vr::VREvent_t event;
    std::vector<vr::VREvent_t> events;
    while (vr::VRServerDriverHost()->PollNextEvent(&event, sizeof(event)))
    {
        events.push_back(event);
    }

    // Update devices
    for (auto& device : m_devices)
        device->update(events);
}

bool TutorialDriver::DeviceProvider::ShouldBlockStandbyMode()
{
    return false;
}

void TutorialDriver::DeviceProvider::EnterStandby()
{
}

void TutorialDriver::DeviceProvider::LeaveStandby()
{
}
