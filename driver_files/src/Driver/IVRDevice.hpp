#pragma once

#include <openvr_driver.h>
#include <Driver/DeviceType.hpp>

namespace ExampleDriver {
    class IVRDevice : public vr::ITrackedDeviceServerDriver {
    public:
        /// <summary>
        /// Returns the serial string for this device
        /// </summary>
        /// <returns>Device serial</returns>
        virtual std::string GetSerial() = 0;

        /// <summary>
        /// Runs any update logic for this device.
        /// Called once per frame
        /// </summary>
        virtual void Update() = 0;

        /// <summary>
        /// Returns the OpenVR device index
        /// This should be 0 for HMDs
        /// </summary>
        /// <returns>OpenVR device index</returns>
        virtual vr::TrackedDeviceIndex_t GetDeviceIndex() = 0;
        
        /// <summary>
        /// Returns which type of device this device is
        /// </summary>
        /// <returns>The type of device</returns>
        virtual DeviceType GetDeviceType() = 0;

        // Inherited via ITrackedDeviceServerDriver
        virtual vr::EVRInitError Activate(uint32_t unObjectId) = 0;
        virtual void Deactivate() = 0;
        virtual void EnterStandby() = 0;
        virtual void* GetComponent(const char* pchComponentNameAndVersion) = 0;
        virtual void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) = 0;
        virtual vr::DriverPose_t GetPose() = 0;

        ~IVRDevice() = default;
    };
};