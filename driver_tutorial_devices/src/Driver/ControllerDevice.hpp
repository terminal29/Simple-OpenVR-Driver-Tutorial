#pragma once

#include <chrono>
#include <cmath>

#include <linalg.h>

#include <Driver/IVRDevice.hpp>

namespace TutorialDriver {
    class ControllerDevice : public IVRDevice {
        public:
            ControllerDevice(std::string serial);
            ~ControllerDevice() = default;

            // Inherited via IVRDevice
            virtual std::string serial() override;
            virtual void update(std::vector<vr::VREvent_t> events) override;
            virtual vr::TrackedDeviceIndex_t device_index() override;
            virtual vr::EVRInitError Activate(uint32_t unObjectId) override;
            virtual void Deactivate() override;
            virtual void EnterStandby() override;
            virtual void* GetComponent(const char* pchComponentNameAndVersion) override;
            virtual void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) override;
            virtual vr::DriverPose_t GetPose() override;

    private:
        vr::TrackedDeviceIndex_t m_deviceIndex = vr::k_unTrackedDeviceIndexInvalid;
        std::string m_serial;

        double m_x = 0, m_y = 0, m_z = 0;
        double m_xRot = 0, m_yRot = 0;

        std::chrono::system_clock::time_point m_lastFrameTime;
    

    };
};