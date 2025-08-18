cmake_minimum_required(VERSION 3.10)
project(cflex C)

# Configure custom output directories for binaries
# This places executables in /bin and libraries in /lib, outside the build folder

# Set default output directories for single-config builds (e.g., Makefiles)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../lib")

# For multi-config builds (e.g., Visual Studio), set per-config directories
foreach(OUTPUT_CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUT_CONFIG} OUTPUT_CONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG_UPPER}	"${CMAKE_BINARY_DIR}/../bin/${OUTPUT_CONFIG}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_CONFIG_UPPER}  	"${CMAKE_BINARY_DIR}/../lib/${OUTPUT_CONFIG}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUT_CONFIG_UPPER}  	"${CMAKE_BINARY_DIR}/../lib/${OUTPUT_CONFIG}")
endforeach()

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
set_source_files_properties(src/cflex_build/internal/cflex_platform.c PROPERTIES HEADER_FILE_ONLY ON)

# Organize build tool files in the IDE to mirror the directory structure
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex_build PREFIX "source" FILES ${TOOL_SOURCE_FILES})


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


# --- cflex library (header-only) ---
# Create an INTERFACE library for cflex. This is the modern CMake way to handle
# header-only libraries, as it allows usage requirements (like include paths)
# to be attached to the library and propagated to consumers.
add_library(cflex INTERFACE)

# The cflex library needs its own source directory for its header, and the
# generated files directory for the generated code, to be on its public include path.
# This ensures that any target linking against cflex can find its headers.
target_include_directories(cflex INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex
    ${GENERATED_DIR}
)


# --- Example Application ---
# This is the final executable that demonstrates the library.
file(GLOB_RECURSE PROGRAM_SOURCE_FILES "src/program/*.c" "src/program/*.h")

add_executable(program ${PROGRAM_SOURCE_FILES})

# The program needs to include headers from its own source directory.
target_include_directories(program PRIVATE src/program)

# Link the program against the cflex library. This transitively applies the
# include directories from cflex to the program, which is a more robust way
# of managing dependencies for IDEs and build systems.
target_link_libraries(program PRIVATE cflex)

# Crucially, the 'program' target must depend on the 'generate_reflection' target.
# This ensures that the generated files are created *before* the program is compiled.
add_dependencies(program generate_reflection)


# --- IDE File Organization ---
# We still want to see the library and generated files in the IDE.
set(LIB_HEADER_FILES
    src/cflex/cflex.h
    src/cflex/cflex_implementation.h
)

# Organize application and library files in the IDE to mirror the directory structure
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/program PREFIX "program" FILES ${PROGRAM_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex PREFIX "cflex" FILES ${LIB_HEADER_FILES})
source_group("generated" FILES ${GENERATED_C} ${GENERATED_H})


# --- Optional: Installation ---
# This section provides rules for installing the final application and the library header.
# It is commented out but serves as a good practice example.
# install(TARGETS program DESTINATION bin)
# install(FILES src/cflex/cflex.h DESTINATION include)
