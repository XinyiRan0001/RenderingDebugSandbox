cmake_minimum_required(VERSION 3.16)

project(RenderingDebugSandbox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(RenderingDebugSandbox
    src/app/main.cpp
    src/rasterizer/RasterizerDemo.cpp
    src/rasterizer/Triangle.cpp
    src/rasterizer/Framebuffer.cpp
    src/app/RopeVisualDemo.cpp
    src/simulation/rope/rope.cpp
)

target_include_directories(RenderingDebugSandbox PRIVATE
    src/app
    src/core
    src/simulation/rope
)

target_link_libraries(RenderingDebugSandbox PRIVATE
    glfw
    OpenGL::GL
    imgui::imgui
)