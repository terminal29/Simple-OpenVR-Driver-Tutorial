cmake_minimum_required(VERSION "3.7.1")

# Solution
project("driver_example")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Deps
set(OPENVR_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries/openvr/headers")

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

# Example Driver
set(EXAMPLE_PROJECT "driver_example")
file(GLOB_RECURSE HEADERS "${EXAMPLE_PROJECT}/src/*.hpp")
file(GLOB_RECURSE SOURCES "${EXAMPLE_PROJECT}/src/*.cpp")
add_library("${EXAMPLE_PROJECT}" SHARED "${HEADERS}" "${SOURCES}")

target_include_directories("${EXAMPLE_PROJECT}" PUBLIC "${OPENVR_INCLUDE_DIR}")
target_include_directories("${EXAMPLE_PROJECT}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libraries/linalg")
target_include_directories("${EXAMPLE_PROJECT}" PUBLIC "${EXAMPLE_PROJECT}/src/")
target_link_libraries("${EXAMPLE_PROJECT}" PUBLIC "${OPENVR_LIB}")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_PROJECT}/src" PREFIX "Header Files" FILES ${HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_PROJECT}/src" PREFIX "Source Files" FILES ${SOURCES})
set_property(TARGET "${EXAMPLE_PROJECT}" PROPERTY CXX_STANDARD 17)

# Copy driver assets to output folder
add_custom_command(
    TARGET ${EXAMPLE_PROJECT}
    PRE_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_SOURCE_DIR}/${EXAMPLE_PROJECT}/driver/ 
    $<TARGET_FILE_DIR:${EXAMPLE_PROJECT}>
)

# Copy dll to output folder
add_custom_command(
    TARGET ${EXAMPLE_PROJECT} 
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy 
    $<TARGET_FILE:${EXAMPLE_PROJECT}>
    $<TARGET_FILE_DIR:${EXAMPLE_PROJECT}>/${EXAMPLE_PROJECT}/bin/${PLATFORM_NAME}${PROCESSOR_ARCH}/$<TARGET_FILE_NAME:${EXAMPLE_PROJECT}>
)