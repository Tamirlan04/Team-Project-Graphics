# Night Drive - Clustered Forward+ Lighting Plan

Author: Tamerlan Murat
Date: 08.11.2026
Topic: Low-poly Night Drive

## Current Lighting
- OpenGL 3.3 Core
- Blinn-Phong shading
- 16 street lamps
- 2 car headlights
- Directional moonlight
- Brute-force lighting in scene.frag

## Current Limitations
- MAX_STREET = 16
- Every fragment checks all street lights
- Clustered Forward+ is not implemented yet

## Planned Implementation
1. Divide the camera view into 3D clusters.
2. Determine light-cluster intersections on CPU.
3. Create light index lists for each cluster.
4. Upload light data to GPU texture buffers.
5. Select clusters in the GLSL fragment shader.
6. Calculate lighting using relevant lights only.

## Cluster Grid
16 x 9 x 8 = 1152 clusters

## Planned Files
- nightdrive/src/clustered_lighting.h
- nightdrive/src/clustered_lighting.cpp
- nightdrive/shaders/scene.frag

## Progress
08 Nov - Lighting architecture documented
09 Nov - Cluster structures
10 Nov - CPU light culling
11 Nov - GLSL integration
12 Nov - Testing
13 Nov - Final lighting checks