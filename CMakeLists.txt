cmake_minimum_required(VERSION 3.31)
project(opengl-renderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall")

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(THIRDPARTY_SRC_DIR "${CMAKE_SOURCE_DIR}/thirdparty/src")
set(THIRDPARTY_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/include")
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")

file(GLOB_RECURSE SRC_FILES
        "${SRC_DIR}/*.cpp"
        "${SRC_DIR}/*.c"
        "${THIRDPARTY_SRC_DIR}/*.cpp"
        "${THIRDPARTY_SRC_DIR}/*.c")

add_executable(main ${SRC_FILES}
        include/electrons.h
        include/scene.h)

target_include_directories(main PRIVATE ${INCLUDE_DIR} ${THIRDPARTY_INCLUDE_DIR})
target_link_directories(main PRIVATE ${LIB_DIR})
target_compile_definitions(main PRIVATE GLFW_STATIC)
target_link_libraries(main PRIVATE glfw3 opengl32 assimp zlibstatic)

target_compile_options(main PRIVATE
        $<$<CONFIG:Debug>:-O0;-g>
        $<$<CONFIG:Release>:-O3>
)

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# Keep textures and shaders next to main.exe
add_dependencies(main copy_assets)
add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
        "${CMAKE_CURRENT_BINARY_DIR}/shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/textures"
        "${CMAKE_CURRENT_BINARY_DIR}/textures"
        COMMENT "Copying shaders and textures..."
)