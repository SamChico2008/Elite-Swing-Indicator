cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(EliteSwingIndicator VERSION 1.0.0)

find_package(Geode REQUIRED)

# Fail-safe: If Geode CLI is missing locally, disable CLI calls to prevent configuration errors.
find_program(GEODE_CLI NAMES geode.exe geode-cli.exe geode geode-cli)
if (NOT GEODE_CLI)
    message(WARNING "Geode CLI not found locally. CLI calls (packaging) will be disabled for this build.")
    set(GEODE_DISABLE_CLI_CALLS ON)
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

setup_geode_mod(${PROJECT_NAME})
