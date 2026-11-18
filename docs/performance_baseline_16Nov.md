\# Night Drive — Baseline Performance Verification



\*\*Date:\*\* November 16, 2026  

\*\*Project:\*\* Night Drive — Topic 7  

\*\*Author:\*\* Tamirlan  

\*\*Role:\*\* Integration, HUD \& Submission Engineer



\## 1. Objective



The objective of this test was to verify the performance and stability of the Night Drive application at a target resolution of 1280×720.



The project requirement is to maintain at least 30 FPS during gameplay.



\## 2. Test Environment



\- Operating System: Windows

\- IDE / Compiler: Visual Studio 2022 / MSVC

\- Build Configuration: Debug

\- Programming Language: C++14

\- Graphics API: OpenGL 3.3 Core Profile

\- Build System: CMake

\- Lighting Mode: Clustered

\- Active Street Lights: 16

\- Target Resolution: 1280×720



\## 3. Performance Test Results



| Test Scenario | Observed FPS | Result |

|---|---|---|

| Vehicle Idle | 104 | Passed |

| Vehicle Driving | 93–100 | Passed |

| Window Resize | Above 100 | Passed |

| Minimum Required FPS | 30 | Achieved |



\## 4. Testing Procedure



The application was launched using the Windows Debug build.



Three performance scenarios were evaluated:



1\. \*\*Idle Test:\*\* The vehicle remained stationary while the FPS counter was observed.

2\. \*\*Driving Test:\*\* The vehicle was driven along the road while monitoring FPS.

3\. \*\*Window Resize Test:\*\* The application window was resized to observe rendering stability and FPS behavior.



The clustered lighting mode was displayed in the HUD during testing.



\## 5. Observations



\- The application launched successfully.

\- The night city, road, vehicle, buildings, and street lights were rendered.

\- The HUD displayed real-time FPS and vehicle speed.

\- The observed FPS was approximately 104 when stationary.

\- FPS remained between 93 and 100 during driving.

\- FPS exceeded 100 in the observed resized-window state.

\- The observed performance exceeded the minimum requirement of 30 FPS.



These results are manual observations, not automated benchmark averages or guaranteed minimum frame rates.



\## 6. Conclusion



The Night Drive application successfully met the performance target during the observed tests.



At the tested 1280×720 resolution, the recorded idle and driving FPS values were above the required 30 FPS.



The window resize test also showed performance above 100 FPS in the observed resized state.



The baseline performance verification for November 16, 2026, was completed without modifying the application's source code.

