# Night Drive — Lighting Benchmark Comparison

**Task date:** November 17, 2026  
**Project:** Night Drive — Topic 7  
**Engineer:** Tamirlan  
**Role:** Integration, HUD & Submission Engineer  
**Report type:** Manual comparative FPS measurements (documentation only)

## 1. Objective

Compare the observed performance of the **BRUTE-FORCE** and **CLUSTERED** lighting modes with **16, 32, and 64 street lights**, using the same test conditions for each paired observation. The project's frame-rate target is **at least 30 FPS at 1280 × 720**.

## 2. Test conditions and method

- **Resolution:** 1280 × 720.
- **Vehicle state:** Stationary (idle) during all six measurements.
- **Other conditions:** Kept the same between the two modes for each street-light count, as reported by the tester.
- **Independent variables:** Lighting mode (BRUTE-FORCE / CLUSTERED) and selected street-light count (16 / 32 / 64).
- **Recorded metric:** FPS shown in Night Drive.
- **Derived metric:** Frame time, calculated as `1000 / FPS` milliseconds.
- **Limitations:** Results are single manual observations, not multi-run averages or measured GPU frame timings. Exact hardware specifications, frame-time distribution and test duration were not recorded. The visible mode label alone does not prove that GPU clustered-light processing is active internally.

## 3. Results

| Street lights | BRUTE-FORCE (FPS) | BRUTE-FORCE frame time (ms) | CLUSTERED (FPS) | CLUSTERED frame time (ms) | CLUSTERED vs. BRUTE-FORCE (FPS) |
|---:|---:|---:|---:|---:|---:|
| 16 | 215 | 4.65 | 216 | 4.63 | +1 (+0.47%) |
| 32 | 134 | 7.46 | 133 | 7.52 | −1 (−0.75%) |
| 64 | 76 | 13.16 | 77 | 12.99 | +1 (+1.32%) |

**Calculation:** `Frame time (ms) ≈ 1000 / FPS`.

**Example:** At 64 lights in BRUTE-FORCE mode, `1000 / 76 ≈ 13.16 ms` per frame. Frame times are calculated estimates based on the displayed FPS, not independently instrumented measurements.

## 4. Interpretation

1. **Both modes exceeded 30 FPS** at every tested light count; the lowest recorded value was 76 FPS (64 lights, BRUTE-FORCE).
2. **FPS declined as the selected light count increased** in both modes: 215 → 134 → 76 for BRUTE-FORCE, and 216 → 133 → 77 for CLUSTERED.
3. **The two modes performed almost identically** in these observations. Differences were only one FPS in each pair (approximately 0.47%, −0.75%, and 1.32%, respectively). These results do not establish a measurable performance advantage for CLUSTERED.
4. **Further verification is necessary** before claiming a real Clustered Forward+ speedup: verify GPU buffer binding and light-list use, collect repeated measurements and frame-time distributions under consistent hardware and graphics settings, and inspect whether both modes actually execute different shader paths.

## 5. Conclusion

The November 17 lighting benchmark support task produced six paired FPS observations at 1280 × 720. Both BRUTE-FORCE and CLUSTERED exceeded the 30 FPS performance target for 16, 32 and 64 street lights. **No meaningful performance lead can be concluded from the present measurements.** This report documents the observed results without changing any C++, GLSL shader or CMake files.

## 6. Git submission scope

**Proposed documentation file:** `docs/lighting_benchmarks_17Nov.md`  
**Planned commit:** `docs(performance): add lighting benchmarks`

Only this Markdown file should be included in the Pull Request. Before merging, request two teammate reviews and confirm that no gameplay, shader or build files were changed.
