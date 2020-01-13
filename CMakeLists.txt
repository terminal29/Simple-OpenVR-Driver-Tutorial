cmake_minimum_required(VERSION "3.7.1")

# Solution
project("driver_tutorial")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Deps

# HMD Tutorial
file(GLOB_RECURSE HEADERS "driver_tutorial_hmd/*.hpp")
file(GLOB_RECURSE SOURCES "driver_tutorial_hmd/*.cpp")
add_library("driver_tutorial_hmd" SHARED ${HEADERS} ${SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/driver_tutorial_hmd" PREFIX "Header Files" FILES ${HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/driver_tutorial_hmd" PREFIX "Source Files" FILES ${SOURCES})

# Devices Tutorial
file(GLOB_RECURSE HEADERS "driver_tutorial_devices/*.hpp")
file(GLOB_RECURSE SOURCES "driver_tutorial_devices/*.cpp")
add_library("driver_tutorial_devices" SHARED ${HEADERS} ${SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/driver_tutorial_devices" PREFIX "Header Files" FILES ${HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/driver_tutorial_devices" PREFIX "Source Files" FILES ${SOURCES})