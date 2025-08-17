This file provides instructions for AI agents working on this codebase.

## Coding Conventions

- **Language:** Strict C11. Do not use language extensions.
- **Portability:** Code must be written to be portable across platforms (Windows, Linux, macOS).
- **Integer Types:** Use types from `<stdint.h>` (e.g., `uint32_t`, `int64_t`) instead of `int`, `long`, etc., to ensure consistent size and representation.
- **Function Names:** All function names must be `lowercase_with_underscores`.
- **Type Names:** All `struct` type definitions must use a `_t` suffix. For example: `my_struct_t`.
- **Build System:** This project uses CMake. All build-related changes must be made in the `CMakeLists.txt` files.

## Directory Structure

- Any C source file that is not meant to be compiled directly, but is instead included by another `.c` file (i.e., for a unity build), should be placed in a subdirectory named `internal/`. This separates the public-facing source files of a module from its internal implementation details.
