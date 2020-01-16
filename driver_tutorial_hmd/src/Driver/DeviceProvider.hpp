#pragma once

#include <vector>
#include <memory>

#include <openvr_driver.h>

#include <Driver/HMDDevice.hpp>
#include <Driver/IVRDevice.hpp>

namespace TutorialDriver {
    class DeviceProvider : public vr::IServerTrackedDeviceProvider {
    public:
        // Inherited via IServerTrackedDeviceProvider
        virtual vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext) override;
        virtual void Cleanup() override;
        virtual const char* const* GetInterfaceVersions() override;
        virtual void RunFrame() override;
        virtual bool ShouldBlockStandbyMode() override;
        virtual void EnterStandby() override;
        virtual void LeaveStandby() override;
        virtual ~DeviceProvider() = default;
    private:
        std::vector<std::shared_ptr<IVRDevice>> m_devices;
    };
};