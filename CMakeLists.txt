cmake_minimum_required(VERSION 3.10)
include(CMakePrintHelpers)

project(cxx-d3d12 VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} WIN32
    src/DDSTextureLoader.cpp
    src/MathHelper.cpp
    src/d3dUtil.cpp
    src/main.cpp
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
endif()
