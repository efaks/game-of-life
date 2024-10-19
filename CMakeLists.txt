# Minimum CMake version required
cmake_minimum_required(VERSION 3.0)

# Project name and version
project(OpenGLProject VERSION 1.0 LANGUAGES C)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLFW
find_package(glfw3 REQUIRED)

# Add GLEW include and link directories (assuming Homebrew paths)
include_directories(/opt/homebrew/opt/glew/include)
link_directories(/opt/homebrew/opt/glew/lib)

# Add the executable
add_executable(OpenGLProject main.c)

# Link OpenGL, GLFW, and GLEW libraries
target_link_libraries(OpenGLProject ${OPENGL_LIBRARIES} glfw GLEW)

# Set additional compiler flags
set(CMAKE_C_FLAGS "-Wall -g")