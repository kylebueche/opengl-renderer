cmake_minimum_required(VERSION 3.31)
project(opengl-renderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -O3")

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty")
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")

file(GLOB_RECURSE SRC_FILES
        "${SRC_DIR}/*.cpp"
        "${SRC_DIR}/*.c"
        "${THIRDPARTY_DIR}/*.cpp"
        "${THIRDPARTY_DIR}/*.c")

add_executable(main ${SRC_FILES})

target_include_directories(main PRIVATE ${INCLUDE_DIR} ${THIRDPARTY_DIR})
target_link_directories(main PRIVATE ${LIB_DIR})
target_compile_definitions(main PRIVATE GLFW_STATIC)
target_link_libraries(main PRIVATE glfw3 opengl32)

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# ASSIMP
#find_package(ASSIMP REQUIRED)
#message(STATUS "ASSIMP included at ${ASSIMP_INCLUDE_DIR}")
