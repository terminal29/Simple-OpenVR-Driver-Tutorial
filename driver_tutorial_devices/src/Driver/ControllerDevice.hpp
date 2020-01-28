#pragma once

#include <chrono>
#include <cmath>
#include <string>

#include <linalg.h>

#include <Driver/IVRDevice.hpp>

namespace TutorialDriver {
    class ControllerDevice : public IVRDevice {
        public:
            ControllerDevice(std::string serial, bool leftController);
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

        vr::VRInputComponentHandle_t m_compHaptic = vr::k_ulInvalidInputComponentHandle;

        vr::VRInputComponentHandle_t m_compATouch = vr::k_ulInvalidInputComponentHandle;
        vr::VRInputComponentHandle_t m_compAClick = vr::k_ulInvalidInputComponentHandle;

        vr::VRInputComponentHandle_t m_compBTouch = vr::k_ulInvalidInputComponentHandle;
        vr::VRInputComponentHandle_t m_compBClick = vr::k_ulInvalidInputComponentHandle;

        vr::VRInputComponentHandle_t m_compSysTouch = vr::k_ulInvalidInputComponentHandle;
        vr::VRInputComponentHandle_t m_compSysClick = vr::k_ulInvalidInputComponentHandle;

        vr::VRInputComponentHandle_t m_compTriggerValue = vr::k_ulInvalidInputComponentHandle;
        vr::VRInputComponentHandle_t m_compTriggerTouch = vr::k_ulInvalidInputComponentHandle;
        vr::VRInputComponentHandle_t m_compTriggerClick = vr::k_ulInvalidInputComponentHandle;

        vr::VRInputComponentHandle_t m_compGripForce = vr::k_ulInvalidInputComponentHandle;
        vr::VRInputComponentHandle_t m_compGripValue = vr::k_ulInvalidInputComponentHandle;
        vr::VRInputComponentHandle_t m_compGripTouch = vr::k_ulInvalidInputComponentHandle;
        vr::VRInputComponentHandle_t m_compGripClick = vr::k_ulInvalidInputComponentHandle;
        
        vr::VRInputComponentHandle_t m_compTrackpadX = vr::k_ulInvalidInputComponentHandle;
        vr::VRInputComponentHandle_t m_compTrackpadY = vr::k_ulInvalidInputComponentHandle;
        vr::VRInputComponentHandle_t m_compTrackpadTouch = vr::k_ulInvalidInputComponentHandle;
        vr::VRInputComponentHandle_t m_compTrackpadClick = vr::k_ulInvalidInputComponentHandle;
        
        vr::VRInputComponentHandle_t m_compJoystickX = vr::k_ulInvalidInputComponentHandle;
        vr::VRInputComponentHandle_t m_compJoystickY = vr::k_ulInvalidInputComponentHandle;
        vr::VRInputComponentHandle_t m_compJoystickTouch = vr::k_ulInvalidInputComponentHandle;
        vr::VRInputComponentHandle_t m_compJoystickClick = vr::k_ulInvalidInputComponentHandle;
        

        bool m_isLeft = false;

        std::chrono::system_clock::time_point m_lastFrameTime;
    };
};