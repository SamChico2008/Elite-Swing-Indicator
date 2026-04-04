cmake_minimum_required(VERSION 3.25)

project(EliteSwingIndicator)

find_package(Geode CONFIG REQUIRED)

add_geode_mod(
    samchico.swing_arrow
    FILES src/main.cpp
)
