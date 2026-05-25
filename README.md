# Rendering Debug Sandbox

A small interactive graphics and physics sandbox built in C++ using OpenGL, GLFW, and ImGui.

This project combines:
- Rope physics simulation
- Software rasterization
- Z-buffer rendering
- Debug visualization tools
- Interactive rendering experiments

The goal of this project is to explore foundational computer graphics and simulation techniques inspired by GAMES101 and real-world rendering/debugging workflows.

---

# Features

## 1. Rope Simulation Module

- Euler integration
- Verlet integration
- Interactive solver switching
- Velocity visualization
- Simulation timing & FPS debug
- Real-time rope physics

### Euler Solver
![Euler Debug](images/euler_debug.png)

### Verlet Solver
![Verlet Debug](images/verlet_debug.png)

---

## 2. Software Rasterizer Module

Implemented fully on the CPU:

- Triangle rasterization
- Edge-function inside-triangle test
- Barycentric interpolation
- Z-buffer depth testing
- Wireframe rendering
- 2x2 MSAA anti-aliasing
- Interactive triangle rotation
- CPU framebuffer rendering
- Multiple debug visualization modes

---

# Rasterizer Debug Views

## Final Color
![Rasterizer Final](images/rasterizer_final_color.gif)

## Depth Buffer Visualization
![Rasterizer Depth](images/rasterizer_depth.gif)

## Wireframe View
![Rasterizer Wireframe](images/rasterizer_wireframe.gif)

## 2x2 MSAA
![Rasterizer MSAA](images/rasterizer_msaa.gif)

---

### 3. Model Renderer Debugger

An interactive OBJ model renderer based on Assignment 3 concepts, extended with runtime render modes and lighting controls.

**Features**
- OBJ model loading
- Interactive model rotation
- Zoom control
- OpenGL depth testing
- Wireframe rendering
- Normal visualization
- Lambert diffuse lighting
- Blinn-Phong shading
- UV visualization
- Texture mapping
- Texture nearest filtering
- Texture linear filtering
- Adjustable light direction
- ImGui render mode panel

![Model Wireframe](images/model_wireframe.gif)
![Model Normal](images/model_normal.gif)
![Model Lambert](images/model_lambert.gif)
![Model Blinn Phong](images/model_blinn_phong.gif)
![Model UV](images/model_uv.gif)
![Model Texture Nearest](images/model_texture_nearest.gif)
![Model Texture Linear](images/model_texture_linear.gif)

---

# Technical Breakdown

## Rope Physics

The rope simulation supports both:
- Explicit Euler integration
- Verlet integration

The simulation includes:
- Spring constraints
- Gravity forces
- Pinned particles
- Real-time debug visualization

---

## Rasterization Pipeline

The software rasterizer includes:

### Triangle Rasterization
Triangles are rasterized on the CPU using edge functions.

### Inside-Triangle Test
Pixel coverage is determined using signed edge equations.

### Barycentric Interpolation
Depth values are interpolated per-pixel using barycentric coordinates.

### Z-Buffer
Per-pixel depth testing is implemented using a custom depth buffer.

### MSAA Anti-Aliasing
2x2 multi-sampling is implemented manually on the CPU.

### Debug Views
The renderer supports:
- Final Color
- Depth Buffer
- Wireframe

### Model Debug Views

The model renderer includes several runtime visualization modes similar to rendering debug tools:

- Wireframe
- Normal view
- UV view
- Lighting view
- Texture filtering comparison

---

# Controls

## Rope Simulation

| Key | Action |
|---|---|
| 1 | Euler Solver |
| 2 | Verlet Solver |
| R | Reset Rope |
| V | Toggle Velocity Visualization |

---

## Rasterizer

| Key | Action |
|---|---|
| 1 | Final Color |
| 2 | Depth Buffer |
| 3 | Wireframe |
| 4 | MSAA View |
| A | Rotate Triangle Left |
| D | Rotate Triangle Right |

---

## Model Renderer
| Key | Action |
|---|---|
| 1 | Wireframe |
| 2 | Normal visualization |
| 3 | Lambert lighting |
| 4 | Blinn-Phong shading |
| 5 | UV visualization |
| 6 | Texture nearest |
| 7 | Texture linear |
| A / D | Rotate Y axis |
| W / S | Rotate X axis |
| Up / Down | Zoom |
| J / L | Adjust light X |
| I / K | Adjust light Y |

---

# Build

## Requirements

- Visual Studio 2022
- CMake
- vcpkg
- OpenGL
- GLFW
- ImGui

## Build Instructions

```bash
mkdir build
cd build

cmake ..

cmake --build . --config Debug
```

Run:

```bash
.\Debug\RenderingDebugSandbox.exe
```

---

# Future Improvements

Planned future features include:

- Texture mapping
- Perspective correct interpolation
- Model loading
- Blinn-Phong shading
- Normal visualization
- Shadow mapping experiments
- GPU rendering comparison
- Deferred rendering experiments

---

# Learning Goals

This project was built as a personal graphics engineering sandbox to better understand:

- Rendering pipelines
- Rasterization
- Depth buffering
- Anti-aliasing
- Simulation systems
- Graphics debugging workflows

Inspired by:
- GAMES101
- Graphics debugging tools
- Real-time rendering pipelines
