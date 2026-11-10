# Night Drive — Day 10: GPU Cluster Buffer Preparation

Contributor: Tamerlan Murat  
Date: 10 November 2026  
Topic 7: Clustered / Forward+ street lights, OpenGL 3.3 Core

## What is implemented today

- `cluster_gpu_packing.h/.cpp`: converts yesterday's `ClusterGrid` output to contiguous integer and float arrays.
- `cluster_gpu_buffers.h/.cpp`: uses OpenGL 3.3 `GL_TEXTURE_BUFFER` and `glTexBuffer` to upload/bind three GPU resources.
- `tests/gpu_pack/`: standalone CPU packing tests (does **not** require GLEW or OpenGL).

### GPU data layout

| Texture buffer | Format | Per-texel content |
|---|---|---|
| `uClusterRanges` | `GL_RG32UI` | `offset, count` per cluster |
| `uClusterIndices` | `GL_R32UI` | one street-light index |
| `uClusterLights` | `GL_RGBA32F` | two `vec4` per light: `worldXYZ, radius`, then `RGB, intensity` |

The cluster grid is 16 × 9 × 8 = 1152 clusters at 1280 × 720. It must be reconfigured when viewport/projection changes. The data needs to be rebuilt and uploaded when the camera or lights move.

## Test CPU packing without GLEW

From repo root:

```powershell
cmake -S nightdrive/tests/gpu_pack -B "T:\NightDrive_GPU_Pack_Build" -G "Visual Studio 18 2026" -A x64
cmake --build "T:\NightDrive_GPU_Pack_Build" --config Release
ctest --test-dir "T:\NightDrive_GPU_Pack_Build" -C Release --output-on-failure
```

## Future OpenGL integration (not done today)

1. Install/configure GLEW and FreeGLUT on Windows (currently game CMake fails at `find_package(GLEW)`).
2. Add `src/clustered_lighting.cpp`, `src/cluster_gpu_packing.cpp` and `src/cluster_gpu_buffers.cpp` to **game** `nightdrive/CMakeLists.txt` (avoid duplicate source entries).
3. After OpenGL context and GLEW init, call `gpu.initialize()`.
4. Every frame: `grid.buildLightLists(viewMatrix, streetLights)`, `packClusterGpuData(grid, streetLights)`, `gpu.upload(packed)`, `gpu.bind(shaderProgram)` **after** `glUseProgram(shaderProgram)`.
5. On shutdown, call `gpu.destroy()` before destroying the GL context.
6. On Nov 11, implement GLSL `usamplerBuffer uClusterRanges`, `usamplerBuffer uClusterIndices`, `samplerBuffer uClusterLights`, cluster lookup and actual per-fragment lighting.
7. Verify GPU limits and frame-rate comparisons; maintain brute-force baseline for the required toggle.

**Current status:** GPU upload implementation prepared, CPU packing verified by separate tests; OpenGL integration and runtime testing pending. Do not mark Forward+ as complete yet.
