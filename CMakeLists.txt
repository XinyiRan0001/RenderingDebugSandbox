cmake_minimum_required(VERSION 3.16)

project(RenderingDebugSandbox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

add_executable(RenderingDebugSandbox
    src/app/main.cpp
    src/rasterizer/RasterizerDemo.cpp
    src/rasterizer/Triangle.cpp
    src/rasterizer/Framebuffer.cpp
    src/model_renderer/ModelRendererDemo.cpp
    src/app/RopeVisualDemo.cpp
    src/simulation/rope/rope.cpp
    src/model_renderer/Texture.cpp
    src/path_tracer/PathTracingDemo.cpp
    src/path_tracer/Renderer.cpp
    src/path_tracer/Scene.cpp
    src/path_tracer/BVH.cpp
    src/path_tracer/Vector.cpp
)

target_include_directories(RenderingDebugSandbox PRIVATE
    src/app
    src/core
    src/simulation/rope
    src/path_tracer
)

target_link_libraries(RenderingDebugSandbox PRIVATE
    glfw
    OpenGL::GL
    imgui::imgui
    Eigen3::Eigen
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})