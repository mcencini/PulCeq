# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)
project(PulSeg)

# Add the pulSeg library as a subdirectory (relative to your source directory)
add_subdirectory(../../csrc ${CMAKE_BINARY_DIR}/pulSeg)  # Avoids 'csrc' folder in 'build'

# Collect all test source files
file(GLOB TEST_SOURCES "*.c")

# Create an executable for the tests
add_executable(run_tests ${TEST_SOURCES})

# Specify the output directory for the compiled test executable
set_target_properties(run_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  # Output in 'build/bin' folder

# Link the test executable with the pulSeg library
target_link_libraries(run_tests PRIVATE pulSeg)

# Ensure that the pulSeg library is built first
add_dependencies(run_tests pulSeg)

# Enable warnings for better code quality
target_compile_options(run_tests PRIVATE -Wall -Wextra)

