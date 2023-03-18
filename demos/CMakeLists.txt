cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE INTERNAL "Default to Debug Build")
endif(NOT DEFINED CMAKE_BUILD_TYPE)

# Set project name to demos
project(demos VERSION 0.0.1 LANGUAGES CXX)

# Import the libhal-stm32 library/package (and all of its dependencies)
find_package(libhal-stm32 REQUIRED CONFIG)

# List of demo applications
# To add a new demo to the list, simply add its filename without the .cpp
# extension to this list.
set(DEMOS output_pin)

# List of targets in the LPC40 series MCU
# TODO: Add targets for LPC17 series MCUs as these libraries are mostly
# compatible
set(TARGETS stm32f103)

# Iterate through each possible target and demo and generate a "make" target
# for each.
foreach(target IN LISTS TARGETS)
    foreach(demo IN LISTS DEMOS)
        set(current_project ${target}_${demo}.elf)
        message(STATUS "Generating Demo for \"${current_project}\"")
        add_executable(${current_project}
            applications/${demo}.cpp
            main.cpp
            newlib.cpp
        )
        target_include_directories(${current_project} PUBLIC .)
        target_compile_options(${current_project} PUBLIC -u _printf_float)
        target_compile_features(${current_project} PRIVATE cxx_std_20)
        target_link_libraries(${current_project} PRIVATE libhal::${target})
        target_link_options(${current_project} PUBLIC -u _printf_float)
        arm_cortex_post_build(${current_project})
    endforeach()
endforeach()
