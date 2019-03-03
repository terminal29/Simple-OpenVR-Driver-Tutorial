#include "ServerDriver.hpp"

ServerDriver* ServerDriver::_instance = nullptr;

ServerDriver::ServerDriver()
{
	launch_debugger();
}

ServerDriver::~ServerDriver() {
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

	InitDriverLog(vr::VRDriverLog());
	DriverLog("===============================================================================\n");
	DriverLog("================================ 03_compositor ================================\n");
	DriverLog("===============================================================================\n");

	_compositor = VirtualCompositor::make_new();
	
	vr::VRServerDriverHost()->TrackedDeviceAdded(_compositor->get_serial().c_str(), vr::TrackedDeviceClass_HMD, _compositor.get());

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
	_compositor->update();
}

bool ServerDriver::ShouldBlockStandbyMode()
{
	return true;
}

void ServerDriver::EnterStandby()
{
}

void ServerDriver::LeaveStandby()
{
}