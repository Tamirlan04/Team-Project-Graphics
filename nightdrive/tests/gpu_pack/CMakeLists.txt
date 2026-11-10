cmake_minimum_required(VERSION 3.10)
project(NightDriveGpuPackingTests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(gpu_packing_tests
    gpu_packing_tests.cpp
    ../../src/cluster_gpu_packing.cpp
    ../../src/clustered_lighting.cpp)
target_include_directories(gpu_packing_tests PRIVATE ../../src)
enable_testing()
add_test(NAME gpu_packing COMMAND gpu_packing_tests)
