cmake_minimum_required(VERSION 3.22)

project(chaturanga VERSION 1.0)

add_executable(chaturanga
    chaturanga.cpp
    commandline.cpp)

include_directories(chaturanga
    $(CMAKE_CURRENT_SOURCE_DIR)/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    )

configure_file(include/version.h.in include/version.h)

target_compile_features(chaturanga PRIVATE cxx_std_23)

