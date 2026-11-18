

\# Windows Clean Build Verification — November 15



\*\*Project:\*\* Night Drive  

\*\*Engineer:\*\* Tamirlan  

\*\*Role:\*\* Integration, HUD \& Submission Engineer



\## Environment

\- Windows 11

\- Visual Studio 2022

\- CMake

\- C++14

\- OpenGL 3.3

\- GLEW and FreeGLUT



\## Build Verification

The project was cloned from the GitHub main branch.



CMake configuration: PASSED  

C++ Debug compilation: PASSED  

Application launch: PASSED



\## Automated Testing

CTest was executed separately from the nightdrive/tests directory.



Result: PASSED (cluster\_grid\_and\_culling).



The root build directory did not contain registered CTest tests.



\## Commands Used

cmake -S nightdrive -B build -A x64

cmake --build build --config Debug



cmake -S nightdrive/tests -B build-tests-15 -A x64

cmake --build build-tests-15 --config Debug

ctest --test-dir build-tests-15 -C Debug --output-on-failure



\## Conclusion

Night Drive was successfully configured, compiled, and launched on Windows. The separately configured cluster grid test passed.



