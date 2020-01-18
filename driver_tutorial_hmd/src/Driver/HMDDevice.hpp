#pragma once

#include <chrono>
#include <cmath>

#include <linalg.h>

#include <Driver/IVRDevice.hpp>

namespace TutorialDriver {
    class HMDDevice : public IVRDevice, public vr::IVRDisplayComponent {
        public:
            HMDDevice(std::string serial);
            ~HMDDevice() = default;

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

            // Inherited via IVRDisplayComponent
            virtual void GetWindowBounds(int32_t* pnX, int32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) override;
            virtual bool IsDisplayOnDesktop() override;
            virtual bool IsDisplayRealDisplay() override;
            virtual void GetRecommendedRenderTargetSize(uint32_t* pnWidth, uint32_t* pnHeight) override;
            virtual void GetEyeOutputViewport(vr::EVREye eEye, uint32_t* pnX, uint32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) override;
            virtual void GetProjectionRaw(vr::EVREye eEye, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom) override;
            virtual vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eEye, float fU, float fV) override;
    private:
        vr::TrackedDeviceIndex_t m_deviceIndex = vr::k_unTrackedDeviceIndexInvalid;
        std::string m_serial;

        uint32_t m_windowX = 0;
        uint32_t m_windowY = 0;
        uint32_t m_windowWidth = 1920;
        uint32_t m_windowHeight = 1080;

        double m_x = 0, m_y = 0, m_z = 0;
        double m_xRot = 0, m_yRot = 0;

        std::chrono::system_clock::time_point m_lastFrameTime;
    

    };
};