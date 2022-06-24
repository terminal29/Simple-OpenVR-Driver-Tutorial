cmake_minimum_required(VERSION "3.15")

# Solution
project("BridgeDriver" CXX)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include("${SUPERPROJECT_SOURCE_DIR}/CMake/shared.cmake")

# Deps
set(OPENVR_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/openvr/headers")

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

find_library(OPENVR_LIB openvr_api HINTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/openvr/lib/${PLATFORM_NAME}${PROCESSOR_ARCH}/" NO_DEFAULT_PATH )

# Example is not updated for changes to IPC
# add_subdirectory("example")

# Example Driver
set(DRIVER_NAME "apriltagtrackers")
set(LIB_NAME "driver_${DRIVER_NAME}")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/driver_files/src/*.hpp")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/driver_files/src/*.cpp")
file(GLOB_RECURSE IPC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/libraries/ipc/*.hpp")
file(GLOB_RECURSE IPC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/libraries/ipc/*.cpp")
add_library("${LIB_NAME}" SHARED "${HEADERS}" "${SOURCES}" "${IPC_HEADERS}" "${IPC_SOURCES}")

target_include_directories("${LIB_NAME}" PUBLIC "${OPENVR_INCLUDE_DIR}")
target_include_directories("${LIB_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libraries/ipc")
target_include_directories("${LIB_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/deps/linalg")
target_include_directories("${LIB_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/driver_files/src/")
target_link_libraries("${LIB_NAME}" PUBLIC "${OPENVR_LIB}")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/driver_files/src" PREFIX "Header Files" FILES ${HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/driver_files/src" PREFIX "Source Files" FILES ${SOURCES})
set_property(TARGET "${LIB_NAME}" PROPERTY CXX_STANDARD 17)
set_property(TARGET "${LIB_NAME}" PROPERTY PREFIX "")

att_read_version_file(DRIVER_VERSION "${CMAKE_CURRENT_SOURCE_DIR}/VERSION.txt")
target_compile_definitions("${LIB_NAME}" PUBLIC ATT_DRIVER_VERSION=${DRIVER_VERSION})

target_link_libraries("${LIB_NAME}" PUBLIC common::semver)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "Install destination." FORCE)
endif()

set(DRIVER_INSTALL_DIR "driver_files")

install(TARGETS "${LIB_NAME}" RUNTIME DESTINATION "${DRIVER_INSTALL_DIR}/${DRIVER_NAME}/bin/${PLATFORM_NAME}${PROCESSOR_ARCH}")

set(DRIVER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/driver_files/driver/apriltagtrackers")
install(DIRECTORY "${DRIVER_DIR}/resources" DESTINATION "${DRIVER_INSTALL_DIR}/${DRIVER_NAME}/resources")
install(FILES "${DRIVER_DIR}/driver.vrdrivermanifest" DESTINATION "${DRIVER_INSTALL_DIR}/${DRIVER_NAME}")

if (WIN32)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/install_driver.bat" DESTINATION "${DRIVER_INSTALL_DIR}")
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/uninstall_driver.bat" DESTINATION "${DRIVER_INSTALL_DIR}")
else()
    # TODO:
    # install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/install_driver.sh" DESTINATION "${DRIVER_INSTALL_DIR}")
endif()
