#include "DriverFactory.hpp"

void* HmdDriverFactory(const char *interface_name, int *return_code) {

	if (std::string(interface_name) == std::string(vr::IServerTrackedDeviceProvider_Version)) {
		ServerDriver* driver = ServerDriver::get();
		if (!driver) {
			*return_code = vr::VRInitError_Init_NotInitialized;
			return nullptr;
		}
		return driver;
	}

	if (return_code)
		*return_code = vr::VRInitError_Init_InterfaceNotFound;

	return nullptr;
}