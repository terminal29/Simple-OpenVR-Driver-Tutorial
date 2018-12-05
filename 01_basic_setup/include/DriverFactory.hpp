#pragma once

#include <string>

#include <openvr_driver.h>

#include "ServerDriver.hpp"

#define EXPORT extern "C" __declspec(dllexport)

EXPORT void* HmdDriverFactory(const char *interface_name, int *return_code);