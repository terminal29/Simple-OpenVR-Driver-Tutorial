# CMakeList.txt : CMake project for HipLocomotion, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)


# Add source to this project's executable.
add_executable (hiplocomotion "hiplocomotion.cpp" "hiplocomotion.h")

target_include_directories("hiplocomotion" PUBLIC "${OPENVR_INCLUDE_DIR}")
target_link_libraries("hiplocomotion" PUBLIC "${OPENVR_LIB}")