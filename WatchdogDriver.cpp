#include "WatchdogDriver.h"

WatchdogDriver::WatchdogDriver()
{
}

WatchdogDriver::~WatchdogDriver()
{
}

EVRInitError WatchdogDriver::Init(IVRDriverContext * pDriverContext)
{
	return EVRInitError::VRInitError_None;
}

void WatchdogDriver::Cleanup()
{
}
