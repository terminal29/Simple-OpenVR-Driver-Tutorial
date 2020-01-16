#include "DeviceProvider.hpp"


vr::EVRInitError TutorialDriver::DeviceProvider::Init(vr::IVRDriverContext* pDriverContext)
{
    // Perform driver context initialisation
    if (vr::EVRInitError init_error = vr::InitServerDriverContext(pDriverContext); init_error != vr::EVRInitError::VRInitError_None) {
        return init_error;
    }


    auto hmdDevice = std::make_shared<HMDDevice>("Tutorial_HMDDevice");
    if(vr::VRServerDriverHost()->TrackedDeviceAdded(hmdDevice->serial().c_str(), vr::ETrackedDeviceClass::TrackedDeviceClass_HMD, hmdDevice.get()))
        this->m_devices.push_back(hmdDevice);

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
