# CMakeList.txt : CMake project for AprilTagTrackers, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)


# Add source to this project's executable.
add_executable (example "example.cpp" "example.h" "../libraries/ipc/Ipc.hpp" "../libraries/ipc/Ipc.cpp")
target_include_directories(example PUBLIC "../libraries/ipc")
