#pragma once
#include <memory>
#include <vector>
#include <chrono>

#include <Driver/IVRDevice.hpp>

namespace ExampleDriver {
    class IVRDriver : protected vr::IServerTrackedDeviceProvider {
    public:

        /// <summary>
        /// Returns all devices being managed by this driver
        /// </summary>
        /// <returns>All managed devices</returns>
        virtual std::vector<std::shared_ptr<IVRDevice>> getDevices() = 0;

        /// <summary>
        /// Returns all OpenVR events that happened on the current frame
        /// </summary>
        /// <returns>Current frame's OpenVR events</returns>
        virtual std::vector<vr::VREvent_t> getOpenVREvents() = 0;

        /// <summary>
        /// Returns the milliseconds between last frame and this frame
        /// </summary>
        /// <returns>MS between last frame and this frame</returns>
        virtual std::chrono::milliseconds getLastFrameTime() = 0;

        virtual inline const char* const* GetInterfaceVersions() override {
            return vr::k_InterfaceVersions;
        };

        virtual ~IVRDriver() {}

    };
}

