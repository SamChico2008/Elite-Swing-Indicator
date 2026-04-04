if (NOT DEFINED GEODE_SDK)
    if (DEFINED ENV{GEODE_SDK})
        set(GEODE_SDK "$ENV{GEODE_SDK}" CACHE PATH "Geode SDK Path")
    else()
        message(FATAL_ERROR "Could not find Geode SDK! Please set the GEODE_SDK environment variable or pass it to CMake.")
    endif()
endif()

if (NOT EXISTS "${GEODE_SDK}/loader/include/Geode/Geode.hpp")
    message(FATAL_ERROR "Geode SDK at ${GEODE_SDK} is invalid! Could not find Geode.hpp.")
endif()

list(APPEND CMAKE_PREFIX_PATH "${GEODE_SDK}")
