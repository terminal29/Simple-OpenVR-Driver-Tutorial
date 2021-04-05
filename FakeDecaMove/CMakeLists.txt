# CMakeList.txt : CMake project for AprilTagTrackers, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)


# Add source to this project's executable.
add_executable (fakemove "fakemove.cpp" "fakemove.h")

target_include_directories("fakemove" PUBLIC "${OPENVR_INCLUDE_DIR}")
target_link_libraries("fakemove" PUBLIC "${OPENVR_LIB}")