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
	for (int i = 0; i < 2; i++) {
		_trackers.push_back(FakeTracker::make_new());
		vr::VRServerDriverHost()->TrackedDeviceAdded(_trackers.back()->get_serial().c_str(), vr::TrackedDeviceClass_GenericTracker, _trackers.back().get());
	}


	return vr::EVRInitError::VRInitError_None;
}

void ServerDriver::Cleanup()
{
}

const char * const * ServerDriver::GetInterfaceVersions()
{
	return vr::k_InterfaceVersions;;
}

void ServerDriver::RunFrame()
{
	for (auto& tracker : _trackers) {
		tracker->update();
	}
	
	vr::VREvent_t event;
	while (vr::VRServerDriverHost()->PollNextEvent(&event, sizeof(event))) {
		for (auto& tracker : _trackers) {
			if (tracker->get_index() == event.trackedDeviceIndex)
				tracker->process_event(event);
		}
	}

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