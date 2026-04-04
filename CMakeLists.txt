cmake_minimum_required(VERSION 3.25)

project(EliteSwingIndicator)

include(Geode.cmake)

find_package(Geode CONFIG REQUIRED)

add_geode_mod(
    samchico.swing_arrow
    FILES src/main.cpp
)

# Standard Geode Linkage
target_link_libraries(samchico.swing_arrow PUBLIC geode::static-conf)
