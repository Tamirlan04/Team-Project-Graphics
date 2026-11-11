# Night Drive - Scene & Gameplay Plan

Author: Temirlan Askaruly (Scene & Gameplay)
Date: 08.11.2026
Topic: Low-poly Night Drive

## Baseline controls (original main.cpp)
| Key | Action |
|---|---|
| Arrow Up / Down | accelerate / reverse |
| Arrow Left / Right | steer |
| Space | brake |
| P | pause |
| R | reset car |
| C | chase camera / orbit camera |
| T | street lights on/off |
| F | headlights on/off |
| [ ] - = | move the moon |
| Mouse drag / wheel | orbit camera / zoom |
| Esc | quit |

## Baseline scene
Straight road (140 m), 16 street lamps, 24 buildings, low-poly red car,
2 headlights, chase and orbit cameras.

## 3 main problems to fix
1. The road ends after 140 m: the car drives onto flat ground (no endless ride).
2. The car can drive through buildings and lamp posts; R is the only recovery.
3. No trip goal or counter; the number of lamps is fixed at 16.

(Verify each problem by running the baseline and attach `docs/screenshots/baseline.png`.)

## Interfaces
- `Car` / `CarInput` (car.h): state, reset, update, headlight position and direction.
- `Scene` (scene.h): buildings, `StreetLight { pos, radius }` list, `setLightPreset(16|32|64)`, `update(carZ)`, `draw()`.
- main.cpp is edited only through the integrator's PR.
