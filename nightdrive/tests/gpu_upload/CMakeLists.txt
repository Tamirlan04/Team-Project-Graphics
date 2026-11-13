cmake_minimum_required(VERSION 3.16)
project(NightDriveGpuUploadTest LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
add_executable(gpu_upload_tests
    gpu_upload_tests.cpp
    ../../src/cluster_gpu_upload.cpp
)
target_include_directories(gpu_upload_tests PRIVATE ../../src)
add_test(NAME GpuUploadValidationTest COMMAND gpu_upload_tests)
