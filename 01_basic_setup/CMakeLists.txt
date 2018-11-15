cmake_minimum_required(VERSION 3.5)

add_library(01_basic_setup SHARED dllmain.cpp)

target_include_directories(01_basic_setup PRIVATE ${OPENVR_INCLUDE_DIR})
target_link_libraries(01_basic_setup ${OPENVR_LIB_DIR}/openvr_api.lib)