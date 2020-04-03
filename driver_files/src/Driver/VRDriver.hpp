#pragma once

#include <vector>
#include <memory>

#include <openvr_driver.h>

#include <Driver/IVRDriver.hpp>
#include <Driver/IVRDevice.hpp>

namespace ExampleDriver {
    class VRDriver : public IVRDriver {
    public:

        // Inherited via IVRDriver
        virtual std::vector<std::shared_ptr<IVRDevice>> getDevices() override;
        virtual std::vector<vr::VREvent_t> getOpenVREvents() override;
        virtual std::chrono::milliseconds getLastFrameTime() override;

        // Inherited via IServerTrackedDeviceProvider
        virtual vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext) override;
        virtual void Cleanup() override;
        virtual void RunFrame() override;
        virtual bool ShouldBlockStandbyMode() override;
        virtual void EnterStandby() override;
        virtual void LeaveStandby() override;
        virtual ~VRDriver() = default;

    private:
        std::vector<std::shared_ptr<IVRDevice>> devices;
        std::vector<vr::VREvent_t> openvrEvents;
        std::chrono::milliseconds frameTiming = std::chrono::milliseconds(16);
        std::chrono::system_clock::time_point lastFrameTime = std::chrono::system_clock::now();

    };
};