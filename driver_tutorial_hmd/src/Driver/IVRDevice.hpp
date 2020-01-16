#pragma once

#include <openvr_driver.h>

#include <Driver/HMDDevice.hpp>

namespace TutorialDriver {
    class IVRDevice : public vr::ITrackedDeviceServerDriver {
    public:
        virtual std::string serial() = 0;
        virtual void update(std::vector<vr::VREvent_t> events) = 0;
        virtual vr::TrackedDeviceIndex_t device_index() = 0;

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