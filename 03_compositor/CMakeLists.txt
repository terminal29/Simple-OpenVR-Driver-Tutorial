cmake_minimum_required(VERSION 3.10)

if(NOT DEFINED OPENVR_INCLUDE_DIR OR NOT DEFINED OPENVR_LIB_DIR)
    message(FATAL_ERROR "Please generate the project file using the CMakeLists file included in the root directory.")
endif()

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp include/*.h)

add_library(03_compositor SHARED ${SOURCES};${HEADERS})
target_include_directories(03_compositor PRIVATE include/)

INCLUDE_EXTERNAL_MSPROJECT( DXUT_2017_Win10 ${CMAKE_CURRENT_SOURCE_DIR}/DXUT/Core/DXUT_2017_Win10.vcxproj PLATFORM WIN32 DXUT_2017_Win10)
add_dependencies(03_compositor DXUT_2017_Win10)

target_include_directories(03_compositor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/DXUT/Core)
target_link_libraries(03_compositor ${CMAKE_CURRENT_SOURCE_DIR}/DXUT/Core/Bin/Desktop_2017_Win10/x64/$<CONFIG>/DXUT.lib)

set_target_properties(03_compositor PROPERTIES PREFIX "driver_")

set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

target_include_directories(03_compositor PRIVATE ${OPENVR_INCLUDE_DIR})
target_link_libraries(03_compositor ${OPENVR_LIB_DIR}/openvr_api.lib)