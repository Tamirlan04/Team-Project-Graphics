# Night Drive — Topic 7

**Computer Graphics Fundamentals | OpenGL 3.3 Core | C++14**

Night Drive is a low-poly nighttime driving demonstration featuring a moving car, an urban road scene, street lights, headlights, two camera modes, and a real-time HUD. The lighting system exposes **BRUTE-FORCE** and **CLUSTERED** modes for comparison.

## Project team and contributions

The project window identifies the participants as **Askaruly Temirlan, Tamerlan Murat, and Tamerlan Kyzylov**. The responsibility labels below follow the team's November task calendar; confirm name spellings against the final submitted report.

| Team role | Responsibilities |
| --- | --- |
| **Tamerlan — Graphics & Lighting Engineer** | Cluster-grid and GPU buffer work, uploading light lists, Forward+ shader and lighting-mode testing, performance benchmarks and lighting documentation. |
| **Temerlan — Scene & Gameplay Engineer** | Vehicle acceleration, braking and steering; continuous road, low-poly buildings, street-lamp placement, cameras, headlights and scene tests. |
| **Tamirlan — Integration, HUD & Submission Engineer** | Windows dependencies and CMake integration; FPS/speed/help HUD; merge and clean-build verification; benchmark documentation; README, report integration and final submission checks. |

## Features

- Resizable low-poly night city with road markings, buildings, street lights, sky and moon.
- Drivable vehicle with chase and orbit cameras, pause and reset.
- Adjustable headlight and street-light visibility.
- On-screen HUD: project information, FPS, speed, lighting mode and controls help.
- Selectable **16, 32, or 64** street lights and lighting-mode toggle.
- Cluster-grid CPU/GPU support and GLSL 330 shaders.

## Requirements (Windows)

- Windows 10/11, 64-bit.
- Visual Studio 2022 with the **Desktop development with C++** workload (MSVC compiler).
- CMake **3.10 or newer**.
- GPU/driver with **OpenGL 3.3 Core Profile** support.
- **GLEW** and **FreeGLUT**, installed with [vcpkg](https://github.com/microsoft/vcpkg).

### Install dependencies (PowerShell)

If vcpkg is installed at `$HOME\vcpkg`, run:

```powershell
& "$HOME\vcpkg\vcpkg.exe" install glew:x64-windows freeglut:x64-windows
```

If vcpkg is in another directory, adjust the paths in the commands below.

## Build and run (Windows PowerShell)

Run these commands from the **repository root**, the folder containing `nightdrive` and `README.md`:

```powershell
cmake -S nightdrive -B "$HOME\NightDrive-readme-build" -A x64 "-DCMAKE_TOOLCHAIN_FILE=$HOME\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake --build "$HOME\NightDrive-readme-build" --config Debug
cd "$HOME\NightDrive-readme-build\Debug"
.\nightdrive.exe
```

The CMake post-build command copies `nightdrive/shaders` next to `nightdrive.exe` into the build output. **Run the executable from its Debug directory** so shader paths resolve correctly. If GLSL compilation fails, read the console error before changing any files.

The initial window size in the source is **1280 × 720**; this is a windowed resolution, not a requirement to use fullscreen.

## Controls

| Key / input | Action |
| --- | --- |
| **Up / Down arrows** | Accelerate / decelerate |
| **Left / Right arrows** | Steer |
| **Space** | Brake |
| **P** | Pause / resume |
| **R** | Reset vehicle |
| **C** | Switch chase / orbit camera |
| **Mouse drag** | Orbit the camera |
| **Mouse wheel** | Zoom orbit camera |
| **H / F1** | Toggle help overlay |
| **L** | Toggle BRUTE-FORCE / CLUSTERED lighting |
| **1 / 2 / 3** | Select 16 / 32 / 64 street lights |
| **T** | Toggle street lights |
| **F** | Toggle headlights |
| **[ / ]** | Adjust moon azimuth |
| **- / +** | Adjust moon elevation |
| **Esc** | Exit |

**Note:** The current code uses **L**, not G, for the lighting-mode toggle.

## Tests

The core game's CMake target and the CPU cluster test suite are configured **separately**. Running CTest against the game build may show `No tests were found`; this does **not** mean that the separately configured tests have failed or passed.

From the repository root:

```powershell
cmake -S nightdrive/tests -B "$HOME\NightDrive-tests-build" -A x64
cmake --build "$HOME\NightDrive-tests-build" --config Debug
ctest --test-dir "$HOME\NightDrive-tests-build" -C Debug --output-on-failure
```

The separate suite registers the `cluster_grid_and_culling` test. Other test subdirectories may require their own CMake configurations; do not interpret one passed test as evidence that all project tests passed.

## Recorded performance (manual observations)

On a Windows Debug build at **1280 × 720**, the team recorded the following FPS results with the vehicle stationary and comparable settings between paired modes:

| Selected street lights | BRUTE-FORCE | CLUSTERED |
| ---: | ---: | ---: |
| 16 | 215 FPS | 216 FPS |
| 32 | 134 FPS | 133 FPS |
| 64 | 76 FPS | 77 FPS |

A separate baseline driving observation recorded **104 FPS idle**, **93–100 FPS driving**, and **over 100 FPS** in the resized-window state with 16 lights and the CLUSTERED label visible. The project target is **at least 30 FPS at 1280 × 720**.

These are **manual observations, not multi-run benchmark averages or GPU timings**. The measurements show similar FPS in the two modes and do **not** independently establish a Clustered Forward+ speedup.

## Documentation and evidence

- [Build dependencies and Windows configuration](docs/build_dependencies.md)
- [Baseline performance, November 16](docs/performance_baseline_16Nov.md)
- [Lighting-mode benchmark, November 17](docs/lighting_benchmarks_17Nov.md)
- [Technical report in Word format](Report.docx)

The November 18 technical draft and final demo media should be linked here **after their files are merged and verified**. Do not add unverified or placeholder download links to the final submission.

## Safe contribution workflow

Create a feature/documentation branch, make the intended change, verify `git diff --cached --name-only`, commit, push, and open a pull request. Request reviews from the two designated teammates and merge **only after both approvals**. For this README task, the PR should change **only `README.md`**—not the game source, shaders, CMake files, or generated build directories.
