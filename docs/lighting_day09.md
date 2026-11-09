# Night Drive — 09 November 2026: CPU cluster grid

Contributor: Tamerlan Murat

## What this patch adds

- `nightdrive/src/clustered_lighting.h`: `PointLight`, `ClusterConfig`, `ClusterBounds`, `ClusterRange`, `ClusterGrid`.
- `nightdrive/src/clustered_lighting.cpp`: creates 16x9x8 conservative view-space bounding boxes using 8 logarithmic depth slices, maps pixels to cells, and builds compact `(offset, count)` + flat light-index lists.
- `nightdrive/tests/`: a standalone CMake + CTest target (no OpenGL dependencies) covering 1,152 cells, boundary mapping, light lists, invalid configuration, camera movement, and resizing.

## Design decisions

Camera space looks along negative Z. `viewDepth = -viewPosition.z` is a positive distance. The grid's Y axis starts at the bottom of the viewport, matching `gl_FragCoord`. Per-cluster AABBs conservatively encompass each frustum cell: extra false-positive light references are allowed, but visible relevant lights are not excluded by the geometric test. Light positions are transformed with the project's existing `mat4` and `transformPoint` in `mathutil.h`.

Current CPU assignment scans every light for every cluster (`O(clusters * lights)`), suitable for checking correctness and as a reference before optimizing candidate selection. For a moving car/camera, `buildLightLists(viewMatrix, lights)` must be called each frame once the renderer is connected.

## Test commands from the repository root

```powershell
cmake -S nightdrive/tests -B build-lighting
cmake --build build-lighting --config Release
ctest --test-dir build-lighting -C Release --output-on-failure
```

## Not yet implemented

The existing `main.cpp`, shader uniforms, and `scene.frag` are intentionally unchanged. GPU texture-buffer upload, GLSL cluster lookup, and the `G` brute-force/clustered toggle are future integration tasks. This commit alone does NOT implement the complete required Forward+ render path.

## Next step

Prepare `GL_TEXTURE_BUFFER` data upload for `(offset,count)` pairs and light indices, coordinate interfaces with the teammate integrating `main.cpp` and CMake, then add GLSL `texelFetch`-based cluster lookup.
