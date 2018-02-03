#pragma once

#include <algorithm>
#include <chrono>
using namespace std::chrono;

#include <openvr_driver.h>
using namespace vr;

#include "DummyController.h"
#include "DummyHMD.h"

class ServerDriver : public IServerTrackedDeviceProvider
{
public:
	ServerDriver();

	~ServerDriver();

	EVRInitError Init(IVRDriverContext *pDriverContext) override;

	void Cleanup() override;

	const char * const *GetInterfaceVersions() override;

	void RunFrame() override;

	bool ShouldBlockStandbyMode() override;
    
	void EnterStandby() override;

	void LeaveStandby() override;

private:
	DummyHMD hmd;

	DummyController controller_left;

	DummyController controller_right;
};

