# cflex

`cflex` is a C11 project that provides reflection capabilities for C programs. It consists of two main parts:

1.  **`cflex_build` tool:** A command-line tool that scans a project's source directory and generates C source files (`cflex_generated.c` and `cflex_generated.h`) containing reflection data.
2.  **`cflex` library:** A single-header, single-compilation-unit library (`cflex.h` and `cflex.c`) that provides data structures and functions to access the generated reflection data.

This project is built using CMake and is designed to be cross-platform.

## Structure

-   `src/cflex_build`: Source code for the build tool.
-   `src/cflex`: The reflection library.
-   `src/program`: An example application demonstrating the usage of `cflex`.
-   `CMakeLists.txt`: The main build script.

## Building

To build the project, use CMake:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```
