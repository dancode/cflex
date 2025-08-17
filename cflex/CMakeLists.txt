cmake_minimum_required(VERSION 3.10)
project(cflex C)

# Set C11 as the standard, require it, and disable extensions for strictness
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Add platform-agnostic warnings, and handle MSVC separately
if(MSVC)
    # /W4 is a high warning level, /WX treats warnings as errors.
    add_compile_options(/W4 /WX)
else()
    # -Wall -Wextra are common warning flags, -pedantic enforces standard compliance,
    # and -Werror treats warnings as errors.
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# --- cflex build tool ---
# This executable is built first. It will be used to generate reflection code.
add_executable(cflex_build src/cflex_build/cflex_build.c)
target_include_directories(cflex_build PUBLIC src/cflex_build)

# --- Generated files ---
# Define the output directory for generated files within the build tree
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Define the full paths for the generated header and source files
set(GENERATED_H ${GENERATED_DIR}/cflex_generated.h)
set(GENERATED_C ${GENERATED_DIR}/cflex_generated.c)

# Add a custom command to run the cflex_build tool.
# This command executes the cflex_build target, passing it the source directory
# of the example program and the directory for the generated output.
add_custom_command(
    OUTPUT ${GENERATED_C} ${GENERATED_H}
    COMMAND cflex_build ${CMAKE_SOURCE_DIR}/src/program ${GENERATED_DIR}
    DEPENDS cflex_build
    COMMENT "Generating reflection files with cflex_build"
    VERBATIM
)

# Add a custom target to make the generation step an explicit part of the build process.
# Making it ALL ensures it runs on a default build.
add_custom_target(generate_reflection ALL DEPENDS ${GENERATED_C} ${GENERATED_H})

# --- Example Application ---
# This is the final executable that demonstrates the library.
# It includes its own source, the cflex library implementation, and the generated source file.
add_executable(program src/program/program.c src/cflex/cflex.c ${GENERATED_C})

# The program needs to include headers from several locations:
# - its own directory
# - the cflex library directory
# - the directory containing the generated headers
target_include_directories(program PRIVATE
    src/program
    src/cflex
    ${GENERATED_DIR}
)

# Crucially, the 'program' target must depend on the 'generate_reflection' target.
# This ensures that the generated files are created *before* the program is compiled.
add_dependencies(program generate_reflection)

# --- Optional: Installation ---
# This section provides rules for installing the final application and the library header.
# It is commented out but serves as a good practice example.
# install(TARGETS program DESTINATION bin)
# install(FILES src/cflex/cflex.h DESTINATION include)
