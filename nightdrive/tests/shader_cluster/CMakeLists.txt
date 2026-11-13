cmake_minimum_required(VERSION 3.16)
project(NightDriveShaderClusterTest LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
add_executable(shader_cluster_tests shader_cluster_tests.cpp)
add_test(NAME ForwardPlusShaderTest COMMAND shader_cluster_tests)
