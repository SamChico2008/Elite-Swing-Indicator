cmake_minimum_required(VERSION 3.21)

# REINFORCED FAIL-SAFE:
# If you don't have the Geode CLI, this forces CMake to ignore it and continue 
# with configuration so you can use IntelliSense and develop locally.
set(GEODE_CLI "disabled" CACHE STRING "Forced Geode CLI path" FORCE)
set(GEODE_DISABLE_CLI_CALLS ON CACHE BOOL "Forced Disable Geode CLI" FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(EliteSwingIndicator VERSION 1.0.0)

find_package(Geode REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

setup_geode_mod(${PROJECT_NAME})
