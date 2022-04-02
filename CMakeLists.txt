cmake_minimum_required(VERSION 3.10)

# Project name
project(qoi-image-tools)

# C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Generate compile_commands.json for YouCompleteMe (YCM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Specify include directories
include_directories(deps/stbi include)

# Set SOURCES to contain all the source files
set(SOURCES
    src/Main.cpp
    src/QOIDecoder.cpp
)

# Executable
add_executable(qoi-image-tools ${SOURCES})
