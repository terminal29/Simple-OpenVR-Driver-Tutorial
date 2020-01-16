#include "DriverFactory.hpp"

static TutorialDriver::DeviceProvider gTrackedDeviceProvider;

void* HmdDriverFactory(const char* interface_name, int* return_code) {
	if (std::strcmp(interface_name, vr::IServerTrackedDeviceProvider_Version) == 0) {
		return &gTrackedDeviceProvider;
	}

	if (return_code)
		*return_code = vr::VRInitError_Init_InterfaceNotFound;

	return nullptr;
}