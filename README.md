# Rendering Debug Sandbox

A C++ real-time rendering and physics debugging sandbox built with OpenGL, GLFW, and ImGui.

This project is designed as a rendering / engine programming portfolio project focused on:

- Physics simulation debugging
- Debug visualization systems
- Runtime profiling
- Fixed timestep simulation
- Rendering experimentation
- Engine-style tooling workflows

---

# Current Module

## Rope Simulation Debug Module

A rope physics simulation inspired by GAMES101 Assignment 8, expanded into a real-time interactive debugging sandbox.

The project compares multiple integration methods while visualizing simulation behavior in real time.

---

# Features

## Physics Simulation

- Explicit Euler Integration
- Verlet Integration
- Fixed timestep physics update
- Rope spring constraint simulation

---

## Debug Visualization

- Velocity vector visualization
- Pinned node visualization
- Solver color visualization
- Runtime debug UI

---

## Runtime Systems

- Real-time profiling
- FPS display
- Simulation timing
- Interactive runtime controls

---

# Controls

| Key | Action |
|---|---|
| 1 | Switch to Euler Solver |
| 2 | Switch to Verlet Solver |
| R | Reset Rope |
| V | Toggle Velocity Visualization |

---

# Solver Comparison

## Euler Integration

Euler integration is simple but numerically unstable.

Observed behavior:
- energy drift
- rope stretching
- unstable motion under larger forces

The sandbox visualizes these issues through exaggerated velocity behavior and rope instability.

---

## Verlet Integration

Verlet integration preserves momentum more effectively and produces more stable rope behavior.

Observed behavior:
- smoother rope motion
- more natural swinging
- improved stability
- better momentum preservation

---

# Debug Visualization

## Velocity Vectors

Blue debug lines visualize rope mass movement direction and magnitude in real time.

This allows quick inspection of:
- momentum transfer
- unstable motion
- integration behavior
- constraint response

---

## Pinned Nodes

Pinned constraint nodes are visualized in red.

This helps debug:
- anchor constraints
- rope setup
- constraint stability

---

# Profiling

The sandbox includes lightweight runtime profiling.

Displayed runtime statistics:
- simulation time
- frame time
- FPS
- fixed timestep information

---

# Technical Highlights

## Fixed Timestep Simulation

Physics updates run independently from rendering frame rate.

This avoids:
- frame-rate dependent physics
- inconsistent simulation speed
- unstable update behavior

---

## Interactive Runtime Debugging

The project includes runtime visualization and debugging tools commonly found in engine development workflows.

Examples:
- runtime solver switching
- debug overlays
- profiling panels
- simulation visualization

---

# Technologies

- C++
- OpenGL
- GLFW
- ImGui
- CMake
- vcpkg

---

# Project Structure

```text
src/
├── app/
│   ├── main.cpp
│   ├── RopeVisualDemo.cpp
│   └── RopeVisualDemo.h
│
├── simulation/
│   └── rope/
│       ├── rope.cpp
│       ├── rope.h
│       ├── mass.cpp
│       ├── mass.h
│       ├── spring.cpp
│       └── spring.h
│
├── profiling/
│   └── Timer.h
│
└── core/
```

---

# Screenshots

## Verlet Solver + Velocity Visualization

![Verlet Debug](images/verlet_debug.png)

---

## Euler Solver Comparison

![Euler Debug](images/euler_debug.png)

---

# Build Instructions

## Requirements

- Visual Studio 2022
- CMake
- vcpkg

---

## Install Dependencies

```bash
vcpkg install glfw3:x64-windows
vcpkg install imgui[glfw-binding,opengl3-binding]:x64-windows
```

---

## Build

```bash
mkdir build
cd build

cmake .. ^
-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg-master/vcpkg-master/scripts/buildsystems/vcpkg.cmake

cmake --build . --config Debug
```

---

# Future Work

Planned future modules:

- Software Rasterizer Debugger
- Wireframe Visualization
- Depth Buffer Visualization
- Normal Visualization
- BVH Visualization
- Ray Tracing Debug Views
- GPU/CPU timing instrumentation
- Shader debugging utilities

---

# Motivation

This project was created to deepen understanding of:

- rendering systems
- simulation architecture
- profiling workflows
- real-time graphics programming
- debug visualization systems

while building practical engine-style tooling and rendering workflows in C++.
