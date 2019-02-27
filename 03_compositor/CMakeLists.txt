cmake_minimum_required(VERSION 3.10)

if(NOT DEFINED OPENVR_INCLUDE_DIR OR NOT DEFINED OPENVR_LIB_DIR)
    message(FATAL_ERROR "Please generate the project file using the CMakeLists file included in the root directory.")
endif()

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp include/*.h)

add_library(03_compositor SHARED ${SOURCES};${HEADERS})
target_include_directories(03_compositor PRIVATE include/)

# GLFW for window management
set(GLFW_BUILD_TESTS FALSE)
set(GLFW_BUILD_EXAMPLES FALSE)
add_subdirectory(glfw)
add_dependencies(03_compositor glfw)
target_link_libraries(03_compositor $<TARGET_FILE:glfw> ${OPENGL_gl_LIBRARY})
target_include_directories(03_compositor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)

# GLEW for GL Extensions
set(GLEW_BINARY_URL "https://nchc.dl.sourceforge.net/project/glew/glew/2.1.0/glew-2.1.0-win32.zip")
set(GLEW_DOWNLOAD_PATH "${CMAKE_CURRENT_SOURCE_DIR}/glew.zip")
set(GLEW_EXTRACT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/glew-2.1.0")
if(NOT EXISTS GLEW_DOWNLOAD_PATH)
# Download GLEW 2.1.0 (Latest at time of writing)
    file(DOWNLOAD "${GLEW_BINARY_URL}" "${GLEW_DOWNLOAD_PATH}")
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar "xzf" "${GLEW_DOWNLOAD_PATH}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(03_compositor "${GLEW_EXTRACT_PATH}/lib/Release/x64/glew32s.lib")
target_include_directories(03_compositor PRIVATE "${GLEW_EXTRACT_PATH}/include")
target_compile_definitions(03_compositor PRIVATE "GLEW_STATIC")


set_target_properties(03_compositor PROPERTIES PREFIX "driver_")

set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

target_include_directories(03_compositor PRIVATE ${OPENVR_INCLUDE_DIR})
target_link_libraries(03_compositor ${OPENVR_LIB_DIR}/openvr_api.lib)