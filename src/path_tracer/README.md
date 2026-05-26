# GAMES101 Assignment 7 — Path Tracing, Multi-threading & Microfacet BRDF

## Overview

This project implements a Monte Carlo Path Tracer with BVH acceleration.

Implemented features include:

- Path Tracing
- Direct Lighting
- Indirect Lighting
- Russian Roulette
- BVH Acceleration
- SAH (Surface Area Heuristic)
- Multi-threaded Ray Generation
- Cook-Torrance Microfacet BRDF

The renderer supports global illumination, soft shadows, indirect light bounces, glossy reflections, and physically-based shading.

---

# Path Tracing

Implemented inside:

```cpp
Scene::castRay()
```

The renderer uses Monte Carlo integration to simulate physically-based light transport.

The algorithm includes:

- Direct light sampling
- Indirect bounce lighting
- Recursive ray tracing
- Russian Roulette termination

The final radiance is computed as:

```text
L = L_direct + L_indirect
```

This allows the renderer to produce:

- Global illumination
- Color bleeding
- Soft shadows
- Indirect diffuse reflection

---

# Direct Lighting

Direct lighting is computed by sampling points on emissive objects.

Implementation steps:

1. Sample a point on the light source
2. Generate a shadow ray
3. Check visibility
4. Evaluate BRDF contribution
5. Accumulate radiance contribution

This produces realistic soft shadows and area light behavior.

---

# Indirect Lighting

Indirect lighting is computed recursively using secondary rays.

The renderer:

1. Samples a new reflection direction
2. Traces another ray into the scene
3. Recursively accumulates radiance

This produces:

- Global illumination
- Multi-bounce light transport
- Color bleeding between surfaces

---

# Russian Roulette

Russian Roulette is used to probabilistically terminate recursive ray paths.

This improves performance while preserving unbiased rendering results.

Implementation:

```cpp
if (get_random_float() < RussianRoulette)
```

This prevents infinite recursion and reduces unnecessary ray computations.

---

# BVH Acceleration

Implemented using Bounding Volume Hierarchy (BVH).

The BVH reduces the number of ray-object intersection tests.

Features include:

- Recursive BVH construction
- Bounding box intersection
- BVH traversal
- Closest hit search

This significantly improves rendering efficiency.

---

# SAH Bonus

Implemented Surface Area Heuristic (SAH) splitting for BVH construction.

Instead of splitting objects directly at the midpoint, the SAH method evaluates multiple split positions and estimates traversal cost using bounding box surface areas.

The split with the minimum estimated cost is selected.

The SAH cost function:

```text
Cost = Ct + (SA(L) / SA(P)) * NL + (SA(R) / SA(P)) * NR
```

Where:

- SA(L) = surface area of left child
- SA(R) = surface area of right child
- SA(P) = surface area of parent node
- NL / NR = number of primitives in left/right child

SAH improves traversal efficiency compared to naive midpoint splitting.

---

# Multi-threading Bonus

Multi-threading was applied to the ray generation stage.

The image rows were divided across multiple CPU threads using:

```cpp
std::thread
```

Each thread independently computes a subset of image rows and writes results into different regions of the framebuffer.

To avoid conflicts:

- `std::atomic<int>` was used for progress tracking
- `std::mutex` was used for console progress synchronization

This significantly reduced rendering time compared to the single-threaded implementation.

---

# Microfacet Bonus

Implemented a Cook-Torrance Microfacet BRDF.

The implementation includes:

- GGX Normal Distribution Function (NDF)
- Schlick Fresnel approximation
- Smith Geometry term

The BRDF combines diffuse and specular reflection:

```text
BRDF = Diffuse + Specular
```

Microfacet materials were applied to the Cornell Box objects to demonstrate glossy reflections and physically-based specular highlights.

Compared to diffuse shading, the microfacet material produces:

- Stronger specular highlights
- Glossy reflections
- More realistic light response

---

# Rendering Results

## Path Tracing Result

![Path Tracing](images/binary.png)

---

## Render Time

![Render Time](images/binary_render.png)

---

## Microfacet BRDF Result

![Microfacet](images/Microfacet.png)

---

## Microfacet Render Time

![Microfacet Render](images/Microfacet_render.png)

---

## Multi-threaded Rendering Result

![Multithreading](images/Multithreading.png)

---

## Multi-threading Render Time

![Multithreading Render](images/Multithreading_render.png)

---

# Technologies

- C++
- Monte Carlo Path Tracing
- BVH Acceleration
- SAH Optimization
- Multi-threading
- Physically Based Rendering (PBR)
- Cook-Torrance BRDF

---

# How to Run

Build the project:

```bash
cmake --build .
```

Run:

```bash
.\Debug\RayTracing.exe
```

The rendered image will be saved as:

```text
binary.ppm
```

---

# File Structure

```text
Assignment7/
│
├── Renderer.cpp
├── Scene.cpp
├── BVH.cpp
├── Bounds3.hpp
├── Material.hpp
├── Triangle.hpp
├── main.cpp
├── README.md
│
├── images/
│   ├── binary.png
│   ├── binary_render.png
│   ├── Microfacet.png
│   ├── Microfacet_render.png
│   ├── Multithreading.png
│   └── Multithreading_render.png
```

---

# Author

Developed as part of the GAMES101 Computer Graphics coursework.
