#include "VRDriver.hpp"
#include <Driver/HMDDevice.hpp>

vr::EVRInitError ExampleDriver::VRDriver::Init(vr::IVRDriverContext* pDriverContext)
{
    // Perform driver context initialisation
    if (vr::EVRInitError init_error = vr::InitServerDriverContext(pDriverContext); init_error != vr::EVRInitError::VRInitError_None) {
        return init_error;
    }


    auto hmdDevice = std::make_shared<HMDDevice>("Tutorial_HMDDevice");
    if(vr::VRServerDriverHost()->TrackedDeviceAdded(hmdDevice->getSerial().c_str(), vr::ETrackedDeviceClass::TrackedDeviceClass_HMD, hmdDevice.get()))
        this->devices.push_back(hmdDevice);

	return vr::VRInitError_None;
}

void ExampleDriver::VRDriver::Cleanup()
{
}

void ExampleDriver::VRDriver::RunFrame()
{
    // Collect events
    vr::VREvent_t event;
    std::vector<vr::VREvent_t> events;
    while (vr::VRServerDriverHost()->PollNextEvent(&event, sizeof(event)))
    {
        events.push_back(event);
    }
    this->openvrEvents = events;

    // Update frame timing
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    this->frameTiming = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->lastFrameTime);
    this->lastFrameTime = now;

    // Update devices
    for (auto& device : this->devices)
        device->update();
}

bool ExampleDriver::VRDriver::ShouldBlockStandbyMode()
{
    return false;
}

void ExampleDriver::VRDriver::EnterStandby()
{
}

void ExampleDriver::VRDriver::LeaveStandby()
{
}

std::vector<std::shared_ptr<ExampleDriver::IVRDevice>> ExampleDriver::VRDriver::getDevices()
{
    return this->devices;
}

std::vector<vr::VREvent_t> ExampleDriver::VRDriver::getOpenVREvents()
{
    return this->openvrEvents;
}

std::chrono::milliseconds ExampleDriver::VRDriver::getLastFrameTime()
{
    return this->frameTiming;
}
