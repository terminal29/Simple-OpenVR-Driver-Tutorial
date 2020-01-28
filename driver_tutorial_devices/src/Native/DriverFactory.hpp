#pragma once

#include <cstdlib>

#include <openvr_driver.h>

#include <Driver/DeviceProvider.hpp>

extern TutorialDriver::DeviceProvider gDeviceProvider;

extern "C" __declspec(dllexport) void* HmdDriverFactory(const char* interface_name, int* return_code);