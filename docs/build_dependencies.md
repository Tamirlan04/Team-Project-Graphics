\# Night Drive — Build Dependencies



\## 1. Development Environment



\- Operating System: Windows 11

\- IDE: Visual Studio 2022

\- Language: C++14

\- Build System: CMake 4.4.3

\- Graphics API: OpenGL

\- Libraries: GLEW and FreeGLUT

\- Package Manager: vcpkg



\## 2. Installing Dependencies



The project requires GLEW and FreeGLUT.



Installation command:



&#x20;   vcpkg install glew:x64-windows freeglut:x64-windows



Both dependencies were installed successfully.



\## 3. CMake Configuration



Run from the repository root:



&#x20;   cmake -S nightdrive -B nightdrive/build -A x64 -DCMAKE\_TOOLCHAIN\_FILE="C:/Users/Admin/vcpkg/scripts/buildsystems/vcpkg.cmake"



Result: Configuration and generation completed successfully.



\## 4. Building the Project



Build command:



&#x20;   cmake --build nightdrive/build --config Debug



Result: Build successful. The nightdrive.exe executable was generated.



\## 5. Running the Application



Navigate to:



&#x20;   nightdrive/build/Debug



Run:



&#x20;   ./nightdrive.exe



Result: Night Drive launched successfully.



\## 6. Verification Results



\- CMake configuration: PASSED

\- C++ compilation: PASSED

\- GLEW dependency: PASSED

\- FreeGLUT dependency: PASSED

\- OpenGL initialization: PASSED

\- Shader loading: PASSED

\- Night city rendering: PASSED

\- Vehicle rendering: PASSED

\- Street lamps rendering: PASSED



\## 7. Notes



The application must be launched from a working directory

where the shaders folder is accessible.



The current application uses brute-force baseline lighting.

Clustered Forward+ integration is planned separately.



\## 8. Conclusion



The Night Drive project was successfully configured,

compiled, and launched on Windows using Visual Studio,

CMake, OpenGL, GLEW, and FreeGLUT.

