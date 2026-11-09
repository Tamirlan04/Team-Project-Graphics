cmake_minimum_required(VERSION 3.10)
project(NightDriveClusterTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This standalone test build does NOT require OpenGL, GLEW or FreeGLUT.
add_executable(clustered_lighting_tests
    clustered_lighting_tests.cpp
    ../src/clustered_lighting.cpp
    ../src/clustered_lighting.h
    ../src/mathutil.h)
target_include_directories(clustered_lighting_tests PRIVATE ../src)

enable_testing()
add_test(NAME cluster_grid_and_culling COMMAND clustered_lighting_tests)
