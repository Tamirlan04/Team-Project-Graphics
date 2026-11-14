cmake_minimum_required(VERSION 3.16)
project(NightDriveGpuBuffersTest LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
add_executable(gpu_buffers_tests
    gpu_buffers_tests.cpp
    ../../src/cluster_gpu_packing.cpp
)
target_include_directories(gpu_buffers_tests PRIVATE ../../src)
add_test(NAME GpuBuffersIntegrationTest COMMAND gpu_buffers_tests)
