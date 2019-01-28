#pragma once

#include <GLFW/glfw3.h>

#include <memory>
#include <array>

#include "VirtualCompositor.hpp"
#include "DriverLog.hpp"

#include <openvr_driver.h>

class ServerDriver : public vr::IServerTrackedDeviceProvider{
public:
	static ServerDriver* get();
	~ServerDriver();
	
	ServerDriver(const ServerDriver&) = delete;
	ServerDriver& operator=(const ServerDriver&) = delete;

	virtual vr::EVRInitError Init(vr::IVRDriverContext * pDriverContext) override;
	virtual void Cleanup() override;
	virtual const char * const * GetInterfaceVersions() override;
	virtual void RunFrame() override;
	virtual bool ShouldBlockStandbyMode() override;
	virtual void EnterStandby() override;
	virtual void LeaveStandby() override;

private:
	ServerDriver();

	static ServerDriver* _instance;

	std::shared_ptr<VirtualCompositor> _compositor;

};