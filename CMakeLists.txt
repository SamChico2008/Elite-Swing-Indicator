cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

# Allow CMake to find Geode SDK in CI/CD environments
if (DEFINED ENV{GEODE_SDK})
    file(TO_CMAKE_PATH "$ENV{GEODE_SDK}" GEODE_SDK_CMAKE_PATH)
    list(APPEND CMAKE_MODULE_PATH "${GEODE_SDK_CMAKE_PATH}/cmake")
endif()

project(EliteSwingIndicator VERSION 1.2.0)

find_package(Geode REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

setup_geode_mod(${PROJECT_NAME})
