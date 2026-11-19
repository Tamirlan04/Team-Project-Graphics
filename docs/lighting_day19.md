# Day 19 Report - Forward+ Technical Documentation & Results (19 Nov 2026)
- **Engineer:** Tamerlan (Lighting / Forward+)
- **Commit:** `docs(lighting): add forward plus results`

## 1. Cluster Grid Resolution (16 x 9 x 8) & Depth Slicing
- **Grid Dimensions:** $N_x \times N_y \times N_z = 16 \times 9 \times 8 = 1152$ clusters.
- **Screen Tiles ($X, Y$):** $16 \times 9$ uniform screen-space tiles ($80 \times 80$ px at $1280 \times 720$).
- **Exponential Depth Slices ($Z$):** $8$ slices between $z_{\text{near}} = 0.1$ and $z_{\text{far}} = 1000.0$ for high near-field precision.

## 2. Mathematical Equations
- **Slice Depth Boundary:** $z_k = z_{\text{near}} \cdot \left(\frac{z_{\text{far}}}{z_{\text{near}}}\right)^{k / N_z}, \quad k \in [0, N_z]$
- **3D Cluster Index:** $c_x = \lfloor x_{\text{frag}} / (W / N_x) \rfloor, \; c_y = \lfloor y_{\text{frag}} / (H / N_y) \rfloor, \; c_z = \lfloor \frac{\ln(-z_{\text{view}} / z_{\text{near}})}{\ln(z_{\text{far}} / z_{\text{near}})} N_z \rfloor, \; \text{idx} = c_x + c_y N_x + c_z N_x N_y$
- **Quadratic Attenuation:** $f_{\text{att}}(d, R) = \left(\max\left(0, 1 - \frac{d}{R}\right)\right)^2$

## 3. Clustered Forward+ Pipeline Diagram
```text
[Camera View/Proj & Lights (16 / 32 / 64)]
                 |
                 v
[CPU: 16x9x8 Cluster AABBs & Sphere-AABB Light Culling]
                 |
                 v
[Pack Offsets/Counts (RG32UI) & Light Indices (R32UI)]
                 |
                 v
[OpenGL 3.3 Texture Buffer Objects (glTexBuffer)]
                 |
                 v
[GLSL 330 scene.frag: texelFetch() -> Blinn-Phong Shading]
```

## 4. FPS & Frame Time Comparison (1280x720)
| Light Count | Brute-Force FPS | Brute-Force Frame Time | Clustered Forward+ FPS | Clustered Frame Time |
| :--- | :--- | :--- | :--- | :--- |






| **16 lights** | 52.6 FPS | 19.00 ms | 100.7 FPS | 9.93 ms |
| **32 lights** | 33.7 FPS | 29.67 ms | 86.7 FPS | 11.53 ms |
| **64 lights** | 19.6 FPS | 51.00 ms | 67.9 FPS | 14.73 ms |

## 5. Screenshots





- Store screenshots of Clustered Forward+ (16+ lights) and `G` key mode comparison in `docs/screenshots/`.
