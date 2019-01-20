#include "ServerDriver.hpp"

ServerDriver* ServerDriver::_instance = nullptr;

ServerDriver::ServerDriver()
{
}

ServerDriver* ServerDriver::get()
{
	if (_instance == nullptr)
		_instance = new ServerDriver();
	return _instance;
}

vr::EVRInitError ServerDriver::Init(vr::IVRDriverContext * driver_context)
{
	if (vr::EVRInitError init_error = vr::InitServerDriverContext(driver_context); init_error != vr::EVRInitError::VRInitError_None) {
		return init_error;
	}

	compositor = VirtualCompositor::make_new();
	
	vr::VRServerDriverHost()->TrackedDeviceAdded(compositor->get_serial().c_str(), vr::TrackedDeviceClass_DisplayRedirect, compositor.get());

	return vr::EVRInitError::VRInitError_None;
}

void ServerDriver::Cleanup()
{
}

const char * const * ServerDriver::GetInterfaceVersions()
{
	return vr::k_InterfaceVersions;
}

void ServerDriver::RunFrame()
{

}

bool ServerDriver::ShouldBlockStandbyMode()
{
	return false;
}

void ServerDriver::EnterStandby()
{
}

void ServerDriver::LeaveStandby()
{
}