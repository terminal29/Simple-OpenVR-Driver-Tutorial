cmake_minimum_required(VERSION 3.5)
project(OpenVR_Examples CXX)


set(OPENVR_LIB_DIR ${CMAKE_SOURCE_DIR}/openvr/lib/win64 CACHE STRING "OpenVR .lib directory")
set(OPENVR_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/openvr/headers CACHE STRING "OpenVR include directory")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/D_UNICODE>")
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/DUNICODE>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/D_UNICODE>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/DUNICODE>")

add_subdirectory(01_basic_setup)

add_subdirectory(02_trackers)

add_subdirectory(03_compositor)

# TODO: Add other projects here





