cmake_minimum_required(VERSION 3.10)
project(cflex C)

# Set C11 as the standard, require it, and disable extensions for strictness
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Enable solution folders for better organization in Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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
file(GLOB_RECURSE TOOL_SOURCE_FILES "src/cflex_build/*.c" "src/cflex_build/*.h")
add_executable(cflex_build ${TOOL_SOURCE_FILES})
target_include_directories(cflex_build PUBLIC src/cflex_build)

# Since cflex_platform.c is included directly by cflex_build.c (unity build),
# we must tell CMake not to compile it as a separate translation unit.
# Setting HEADER_FILE_ONLY ensures it's still visible in IDEs like Visual Studio.
set_source_files_properties(src/cflex_build/cflex_platform.c PROPERTIES HEADER_FILE_ONLY ON)

# Organize build tool files in the IDE to mirror the directory structure
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex_build PREFIX "Source" FILES ${TOOL_SOURCE_FILES})


# --- Generated files ---
# Define the output directory for generated files within the build tree
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Define the full paths for the generated header and source file
set(GENERATED_H ${GENERATED_DIR}/cflex_generated.h)
set(GENERATED_C ${GENERATED_DIR}/cflex_generated.c)

# Add a custom command to run the cflex_build tool.
# This command executes the cflex_build target, passing it the source directory
# of the example program and the directory for the generated output.
# Using a generator expression ensures the correct path to the executable is used.
add_custom_command(
    OUTPUT ${GENERATED_C} ${GENERATED_H}
    COMMAND $<TARGET_FILE:cflex_build> ${CMAKE_SOURCE_DIR}/src/program ${GENERATED_DIR}
    DEPENDS cflex_build
    COMMENT "Generating reflection files with cflex_build"
    VERBATIM
)

# Add a custom target to make the generation step an explicit part of the build process.
# This target depends on the generated files, ensuring they are created.
add_custom_target(generate_reflection DEPENDS ${GENERATED_C} ${GENERATED_H})


# --- Example Application ---
# This is the final executable that demonstrates the library.
file(GLOB_RECURSE PROGRAM_SOURCE_FILES "src/program/*.c" "src/program/*.h")
file(GLOB_RECURSE LIB_SOURCE_FILES "src/cflex/*.c" "src/cflex/*.h")
add_executable(program ${PROGRAM_SOURCE_FILES} ${LIB_SOURCE_FILES} ${GENERATED_C})


# The program needs to include headers from several locations:
target_include_directories(program PRIVATE
    src/program
    src/cflex
    ${GENERATED_DIR}
)

# Crucially, the 'program' target must depend on the 'generate_reflection' target.
# This ensures that the generated files are created *before* the program is compiled.
add_dependencies(program generate_reflection)

# Organize application and library files in the IDE to mirror the directory structure
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/program PREFIX "Source" FILES ${PROGRAM_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex PREFIX "Source/Library" FILES ${LIB_SOURCE_FILES})


# --- Optional: Installation ---
# This section provides rules for installing the final application and the library header.
# It is commented out but serves as a good practice example.
# install(TARGETS program DESTINATION bin)
# install(FILES src/cflex/cflex.h DESTINATION include)
