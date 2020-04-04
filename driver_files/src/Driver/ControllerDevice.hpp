#pragma once

#include <chrono>
#include <cmath>

#include <linalg.h>

#include <Driver/IVRDevice.hpp>
#include <Native/DriverFactory.hpp>

namespace ExampleDriver {
    class ControllerDevice : public IVRDevice {
        public:

            enum Handedness {
                HANDEDNESS_LEFT,
                HANDEDNESS_RIGHT,
                HANDEDNESS_ANY
            };

            ControllerDevice(std::string serial, Handedness handedness = HANDEDNESS_ANY);
            ~ControllerDevice() = default;

            // Inherited via IVRDevice
            virtual std::string GetSerial() override;
            virtual void Update() override;
            virtual vr::TrackedDeviceIndex_t GetDeviceIndex() override;
            virtual DeviceType GetDeviceType() override;

            virtual vr::EVRInitError Activate(uint32_t unObjectId) override;
            virtual void Deactivate() override;
            virtual void EnterStandby() override;
            virtual void* GetComponent(const char* pchComponentNameAndVersion) override;
            virtual void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) override;
            virtual vr::DriverPose_t GetPose() override;

    private:
        vr::TrackedDeviceIndex_t device_index_ = vr::k_unTrackedDeviceIndexInvalid;
        std::string serial_;
        Handedness handedness_;
    };
};