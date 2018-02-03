#pragma once

#include <openvr_driver.h>
using namespace vr;

class WatchdogDriver : public IVRWatchdogProvider
{
public:
	WatchdogDriver();

	~WatchdogDriver();

	EVRInitError Init(IVRDriverContext *pDriverContext) override;

	void Cleanup() override;
};

