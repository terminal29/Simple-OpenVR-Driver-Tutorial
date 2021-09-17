#pragma once
#include <windows.h>
#include <vector> 
#include <iostream>
#include <string>
#include <sstream> 
#include <time.h>
#include <openvr.h>
#include <math.h>
#include <filesystem>


std::istringstream SendMove(double x, double y, double rx, double ry, double a, double b);
std::istringstream Send(LPTSTR lpszWrite);
bool GetDigitalActionState(vr::VRActionHandle_t action, vr::VRInputValueHandle_t* pDevicePath = nullptr);
