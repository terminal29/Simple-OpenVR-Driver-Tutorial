cmake_minimum_required(VERSION 3.5)
project(OpenVR_Tutorial)

set(OPENVR_LIB_DIR ${CMAKE_SOURCE_DIR}/openvr/lib/win64 CACHE STRING "OpenVR .lib directory")
set(OPENVR_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/openvr/headers CACHE STRING "OpenVR include directory")

add_subdirectory(01_basic_setup)

# TODO: Add other projects here





