#pragma once
#include <windows.h>
#include <vector> 
#include <iostream>
#include <string>
#include <sstream> 

void Send(int id, double a, double b, double c, double qw, double qx, double qy, double qz);

HANDLE hmdPipe;
std::vector<HANDLE> hpipe;
int pipeNum = 4;
int waitFrames = 90*30;  //at a 90hz HMD, this equals about half a minute
char buffer[1024];
DWORD dwRead;
DWORD dwWritten;