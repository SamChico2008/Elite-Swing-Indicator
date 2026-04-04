cmake_minimum_required(VERSION 3.25)

project(EliteSwingIndicator)

# CI/CD Search Path Fix
if(DEFINED ENV{GEODE_SDK})
    list(APPEND CMAKE_PREFIX_PATH "$ENV{GEODE_SDK}")
endif()

find_package(Geode CONFIG REQUIRED)

add_geode_mod(
    samchico.swing_arrow
    FILES src/main.cpp
)
