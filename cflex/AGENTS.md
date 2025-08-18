This file provides instructions for AI agents working on this codebase.

## Coding Conventions

- **Language:** Strict C11. Do not use language extensions.
- **Portability:** Code must be written to be portable across platforms (Windows, Linux, macOS).
- **Integer Types:** Use types from `<stdint.h>` (e.g., `uint32_t`, `int64_t`) instead of `int`, `long`, etc., to ensure consistent size and representation.
- **Function Names:** All function names must be `lowercase_with_underscores`.
- **Type Names:** All `struct` type definitions must use a `_t` suffix. For example: `my_struct_t`.
- **Build System:** This project uses CMake. All build-related changes must be made in the `CMakeLists.txt` files.

## High Level Project Overview

- The project goal is a strict C11 reflection library (cflex) and an associated code generation tool (cflex_build) -- There are three parts.
	
	cflex_build: A command-line tool that scans a project's source directory and generates C source files (cflex_generated.c and cflex_generated.h) containing reflection data.
	cflex: A header-only runtime library that uses the generated files to provide reflection data to a user's program. It is designed in a "unity build" style, where an implementation header (cflex_implementation.h) is included in one C file.
	program: An example application that demonstrates how to use the cflex library.
	
## Key Architectural Principles

	1. Header-Only Library: The cflex runtime is a header-only library. 
	2. The public API is in cflex.h. The implementation is in cflex_implementation.h and is meant to be included in exactly one user source file.
	3. Unity Build for Internals: Internal library components (like internal/cflex_parser.h) are not compiled directly but are included by cflex_implementation.h.
	4. Modern CMake: We use modern CMake practices (INTERFACE libraries, target_link_libraries) to manage dependencies. Changes to CMakeLists.txt should respect this pattern.	
	
## Build and Verification Workflow

	This section is crucial to ensure the agent can reliably build and test the project.

	1. Clean Build Steps: To build the project from a clean state, run the following commands from the repository root:

	rm -rf build
	mkdir build
	cd build
	cmake ../cflex
	cmake --build .
	
	2. Verification Step: To verify a successful build, run the example program and check for correct output:

	../bin/program
	
	3. IMPORTANT - IDE Integration Workflow (Visual Studio, etc.): This project generates source code (cflex_generated.c/h) during the build process. Many IDEs generate their project files (e.g., .vcxproj) during the initial cmake configuration step, before these files exist. This can cause issues with IntelliSense, "Go to Definition," and file visibility in the project explorer.
	
	The correct, robust workflow to ensure the IDE is fully synchronized is a two-step process:
 
	Run a full build once to ensure the generated files are created on disk: cmake --build .
	Regenerate the project files: Run the configuration step again: cmake . (note the single dot).
	This second step will update the IDE project with the now-existing generated files, resolving any IntelliSense or file visibility problems. Any agent working on this project must be aware of this two-phase generation process.
	
	The important part is ensuring the generated project files are created on disk but running the build tool once.
	If this step is left out, the main program will have no generated data to compile with.
	

	