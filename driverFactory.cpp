#include "driverFactory.h"

HMD_DLL_EXPORT void *HmdDriverFactory(const char *pInterfaceName, int *pReturnCode)
{
	if (0 == _stricmp(IServerTrackedDeviceProvider_Version, pInterfaceName))
	{
		return &server_driver;
	}
	if (0 == _stricmp(IVRWatchdogProvider_Version, pInterfaceName))
	{
		return &watchdog_driver;
	}

	if (pReturnCode)
		*pReturnCode = VRInitError_Init_InterfaceNotFound;

	return NULL;
}