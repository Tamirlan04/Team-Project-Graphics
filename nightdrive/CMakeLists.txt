cmake_minimum_required(VERSION 3.10)
project(NightDrivePrelim CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

find_package(GLUT QUIET)
if(GLUT_FOUND)
  set(GLUT_TARGET GLUT::GLUT)
else()
  find_package(FreeGLUT CONFIG REQUIRED)
  set(GLUT_TARGET FreeGLUT::freeglut)
endif()

add_executable(nightdrive
    src/main.cpp
    src/mathutil.h

    src/clustered_lighting.cpp
    src/clustered_lighting.h

    src/cluster_gpu_packing.cpp
    src/cluster_gpu_packing.h

    src/cluster_gpu_buffers.cpp
    src/cluster_gpu_buffers.h
)
target_link_libraries(nightdrive PRIVATE OpenGL::GL GLEW::GLEW ${GLUT_TARGET})

add_custom_command(TARGET nightdrive POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:nightdrive>/shaders)
