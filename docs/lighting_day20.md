# Day 20 Report - Final Lighting QA & Defense Guide (20 Nov 2026)
- **Engineer:** Tamerlan (Lighting / Forward+)
- **Commit:** `docs(lighting): finalize shader documentation`

## 1. Windows & Release Build Lighting QA
- [x] Verified Windows x64 Release build and CTest test suites.
- [x] Verified lighting and mode switching (`G` key: Brute-Force vs Clustered Forward+) in Release build.
- [x] Demonstrated `G` toggle with 16+ active street lights (presets: 16, 32, 64) and vehicle headlights.

## 2. Clustered Forward+ Defense Explanation
- **Grid & Slicing:** 16 x 9 x 8 = 1152 clusters with screen-space tiles (X, Y) and exponential depth slicing (Z).
- **CPU Culling & GPU TBOs:** Lights are culled against cluster AABBs; `(offset, count)` (`RG32UI`) and light indices (`R32UI`) are bound via OpenGL Texture Buffer Objects (`glTexBuffer`).
- **GLSL 330 Shader (`scene.frag`):** Computes cluster index from fragment coordinates and view-space depth, reads indices via `texelFetch`, and evaluates Blinn-Phong shading only for lights in the cluster.
