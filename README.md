

# voxel-mc-attempt

Fun attempt to make a voxel C++ Minecraft-like prototype focused only on core engine systems with chunked world streaming, CPU meshing, a small client (OpenGL), and a few other features.

**Why?** — Why not! I had some free time to tinker.

---

## Key Features / General Overview

### Chunked world & streaming

* Columns of chunks; compile-time chunk constants (`CHUNK_BITS` controls chunk cubic size, and `CHUNKS_PER_COLUMN` defines world height in chunks per column).
* On-demand loading and unloading of columns with seed increments.
* Separation of concerns between generation, meshing, and rendering.

### Synchronous `ChunkColumn` generation (parallelized with `std::async`)

* Generation is conceptually synchronous but uses `std::async` to spawn many threads and divide work across them.

### CPU meshing with layer separation

* Face culling and mesh generation on the CPU.
* Separate opaque (occluding) and cutout (alpha-tested) mesh layers for correct rendering passes.

### Tile-based texture atlas & UV packing

* Per-tile UV packing computed to minimize texture binds and reduce draw calls.

### GPU resource `BufferPool`

* VBO/EBO reuse to reduce OpenGL allocations and improve streaming performance.

### Frustum culling & distance sorting

* AABB frustum tests and distance-based sorting.
* Render order: occluding → cutout (e.g., leaves).

### Deterministic terrain options

* Seedable Perlin noise generator.
* Simple sine-wave generator available for quick comparisons.

### Voxel interaction

* Hotbar (keyboard + scroll wheel) block selection and input handling.
* Camera movement and looking around.
* Raycast-based block placement and breaking.

### Used tools & dependencies

* C++ & Python
* CMake + vcpkg
* OpenGL, GLFW, GLAD, GLM, spdlog, stb
* Visual Studio profiler to inspect CPU hotspots

---

## What could be done next

* Persistent chunk storage (currently in-memory only).
* Greedy meshing and GPU occlusion queries.
* Multithreaded `ChunkColumn` generation and meshing using thread pools and job queues (blocking with `std::mutex` or non-blocking with `std::atomic`).
* Implement a server component (currently stubbed).
