#pragma once
#include <vector> 
#include <iostream>
#include <string>
#include <sstream> 
#include <time.h>

std::istringstream SendTracker(int id, double a, double b, double c, double qw, double qx, double qy, double qz, double time, double smoothing);
std::istringstream Send(std::string buffer);
void Sync();

int waitFrames = 90*30;  //at a 90hz HMD, this equals about half a minute
