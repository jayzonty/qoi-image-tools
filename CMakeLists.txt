cmake_minimum_required(VERSION 3.10)

# Project name
project(qoi-tools)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Generate compile_commands.json for YouCompleteMe (YCM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Specify include directories
include_directories(deps/glad/include deps/glm/include deps/stbi ${GLFW_INCLUDE_DIRS} .)

# Set SOURCES to contain all the source files
set(SOURCES
    deps/glad/src/glad.c

    tools/ImageViewerApp.cpp
    tools/Main.cpp
)

# Executable
add_executable(qoi-tools ${SOURCES})

# Libraries
target_link_libraries(qoi-tools ${OPENGL_gl_LIBRARY} glfw ${CMAKE_DL_LIBS})