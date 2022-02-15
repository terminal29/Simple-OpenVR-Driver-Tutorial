#pragma once

#include <cstdlib>
#include <memory>

#include <openvr_driver.h>

#include <Driver/IVRDriver.hpp>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

extern "C" EXPORT void* HmdDriverFactory(const char* interface_name, int* return_code);

namespace ExampleDriver {
    std::shared_ptr<ExampleDriver::IVRDriver> GetDriver();
}
