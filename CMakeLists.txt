cmake_minimum_required(VERSION "3.7.1")

# Solution
project("driver_tutorial")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Deps
SET(OPENVR_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries/openvr/headers")

set(SIZEOF_VOIDP ${CMAKE_SIZEOF_VOID_P})
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PROCESSOR_ARCH "64")
else()
    set(PROCESSOR_ARCH "32")
endif()

if(WIN32)
    set(PLATFORM_NAME "win")
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_NAME "linux")
elseif(APPLE)
    set(PLATFORM_NAME "osx")
endif()

find_library(OPENVR_LIB openvr_api HINTS "${CMAKE_CURRENT_SOURCE_DIR}/libraries/openvr/lib/${PLATFORM_NAME}${PROCESSOR_ARCH}/" NO_DEFAULT_PATH )

# HMD Tutorial
file(GLOB_RECURSE HEADERS "driver_tutorial_hmd/*.hpp")
file(GLOB_RECURSE SOURCES "driver_tutorial_hmd/*.cpp")
add_library("driver_tutorial_hmd" SHARED ${HEADERS} ${SOURCES})
target_include_directories("driver_tutorial_hmd" PUBLIC ${OPENVR_INCLUDE_DIR})
target_link_libraries("driver_tutorial_hmd" PUBLIC ${OPENVR_LIB})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/driver_tutorial_hmd" PREFIX "Header Files" FILES ${HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/driver_tutorial_hmd" PREFIX "Source Files" FILES ${SOURCES})

# Devices Tutorial
file(GLOB_RECURSE HEADERS "driver_tutorial_devices/*.hpp")
file(GLOB_RECURSE SOURCES "driver_tutorial_devices/*.cpp")
add_library("driver_tutorial_devices" SHARED ${HEADERS} ${SOURCES})
target_include_directories("driver_tutorial_devices" PUBLIC ${OPENVR_INCLUDE_DIR})
target_link_libraries("driver_tutorial_devices" PUBLIC ${OPENVR_LIB})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/driver_tutorial_devices" PREFIX "Header Files" FILES ${HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/driver_tutorial_devices" PREFIX "Source Files" FILES ${SOURCES})