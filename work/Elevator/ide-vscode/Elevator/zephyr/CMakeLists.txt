cmake_minimum_required(VERSION 3.13.1)
#include(CMakePrintHelpers)

# Path to out-of-tree boards/shield definitions
set(BOARD_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(SHIELD elevator_v2)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(Elevator)

#get_filename_component(configName "${CMAKE_BINARY_DIR}" NAME)
#cmake_print_variables(CMAKE_CURRENT_SOURCE_DIR)
#cmake_print_text("STOP")

# Add out-of-tree driver sources to cmake
add_subdirectory(drivers)

FILE(GLOB app_sources ../src/*.c*)
target_sources(app PRIVATE ${app_sources})
