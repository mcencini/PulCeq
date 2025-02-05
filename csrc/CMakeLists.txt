# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Define the pulSeg library
add_library(pulSeg pulSeg.c)

# Include directories for pulSeg (if any)
target_include_directories(pulSeg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

