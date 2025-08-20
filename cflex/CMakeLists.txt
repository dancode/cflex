# Minimum version of CMake that we require.
# If someone uses an older version, the build will stop with an error.
cmake_minimum_required(VERSION 3.10)

# Define the project name and language, "C" means it's written in C (not C++).
project(cflex C)

# --------------------------------------------------------------------
# OUTPUT DIRECTORIES
# --------------------------------------------------------------------

# Tell CMake where to place the compiled executables (the final .exe or binary).
# By default, CMake puts them inside the "build" folder, but here we move them
# to a sibling folder called "../bin" for cleanliness.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")

# --------------------------------------------------------------------
# COMPILER SETTINGS
# --------------------------------------------------------------------

# Set C11 as the standard, require it, and disable extensions for strictness
set(CMAKE_C_STANDARD 11)

# Require it (don’t silently fall back to older versions).
set(CMAKE_C_STANDARD_REQUIRED ON)

# Disable compiler "extensions" so we stick strictly to C11 only,
set(CMAKE_C_EXTENSIONS OFF)

# Enable solution folders for better organization in Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# --------------------------------------------------------------------
# COMPILER WARNINGS
# --------------------------------------------------------------------

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# --------------------------------------------------------------------
# CFLX REFLECTION LIBRARY FLES
# --------------------------------------------------------------------

# Gather all cflex library header files and group them too.
file(GLOB_RECURSE CFLEX_HEADER_FILES "src/cflex/*.h")

# --------------------------------------------------------------------
# CFLEX BUILD TOOL (the reflection generator itself)
# --------------------------------------------------------------------

# Gather all .c and .h files from the src/cflex_build folder (and subfolders).
file(GLOB_RECURSE TOOL_SOURCE_FILES "src/cflex_build/*.c" "src/cflex_build/*.h")

# Create an executable program called "cflex_build" from those files.
add_executable(cflex_build ${TOOL_SOURCE_FILES})

# Make sure this tool can include headers from its own source folder.
target_include_directories(cflex_build PUBLIC src/cflex_build)

# Mark some source files as "HEADER_FILE_ONLY" for unity compilation.
# They will show up in IDE project trees but won’t actually be compiled.
set_source_files_properties(
    src/cflex_build/internal/cflex_platform.c
    src/cflex_build/internal/cflex_scan.c
    src/cflex_build/internal/cflex_parse.c
    src/cflex_build/internal/cflex_parse_util.c
    src/cflex_build/internal/cflex_parse_struct.c
    src/cflex_build/internal/cflex_parse_enum.c
    src/cflex_build/internal/cflex_output.c
    src/cflex_build/internal/cflex_std.c
    PROPERTIES HEADER_FILE_ONLY ON
)

# Organize the files into a "source" group in IDEs like Visual Studio.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex_build PREFIX "source" FILES ${TOOL_SOURCE_FILES})

# --------------------------------------------------------------------
# Add Cflex Runtime Library files to Cflex_Build directory
# --------------------------------------------------------------------

# Attach them to the program target (they won't compile, but will show up in VS)
target_sources(cflex_build PRIVATE ${CFLEX_HEADER_FILES})

# Put them under a folder called "cflex" in Visual Studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex PREFIX "cflex" FILES ${CFLEX_HEADER_FILES})

# --------------------------------------------------------------------
# GENERATED FILES (reflection data)
# --------------------------------------------------------------------

# All generated files will go inside a special folder inside the build directory.
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/cflex_generated)

# Make sure that folder actually exists.
file(MAKE_DIRECTORY ${GENERATED_DIR})

# --------------------------------------------------------------------
# --- Default Module (basic reflection types) ---
# --------------------------------------------------------------------

set(DEFAULT_MODULE_NAME "cflex_default")
set(DEFAULT_GENERATED_H ${GENERATED_DIR}/${DEFAULT_MODULE_NAME}_generated.h)
set(DEFAULT_GENERATED_C ${GENERATED_DIR}/${DEFAULT_MODULE_NAME}_generated.c)

# Tell CMake how to create these files.
# The "cflex_build" tool is run to generate them (with the command arugments specified)
add_custom_command(
    OUTPUT ${DEFAULT_GENERATED_C} ${DEFAULT_GENERATED_H}
    COMMAND $<TARGET_FILE:cflex_build> ${CMAKE_CURRENT_SOURCE_DIR} ${GENERATED_DIR} --name ${DEFAULT_MODULE_NAME} --default-types-only
    DEPENDS cflex_build
    COMMENT "Generating default reflection files"
    VERBATIM
)

# Define a target called "generate_cflex_default" that depends on these files.
add_custom_target(generate_cflex_default DEPENDS ${DEFAULT_GENERATED_C} ${DEFAULT_GENERATED_H})

# --------------------------------------------------------------------
# --- Program Module (reflection types) ---
# --------------------------------------------------------------------

set(PROGRAM_MODULE_NAME "program")
set(PROGRAM_GENERATED_H ${GENERATED_DIR}/${PROGRAM_MODULE_NAME}_generated.h)
set(PROGRAM_GENERATED_C ${GENERATED_DIR}/${PROGRAM_MODULE_NAME}_generated.c)

# Tell CMake how to create these files.
# The "cflex_build" tool is run to generate them (with the command arugments specified)
add_custom_command(
    OUTPUT ${PROGRAM_GENERATED_C} ${PROGRAM_GENERATED_H}
    COMMAND $<TARGET_FILE:cflex_build> ${CMAKE_CURRENT_SOURCE_DIR}/src/program ${GENERATED_DIR} --name ${PROGRAM_MODULE_NAME}
    DEPENDS cflex_build
    COMMENT "Generating program reflection files"
    VERBATIM
)

# Define a target called "generate_program" that depends on these files.
add_custom_target(generate_program DEPENDS ${PROGRAM_GENERATED_C} ${PROGRAM_GENERATED_H})

# --------------------------------------------------------------------
# --- Unit Test Module (reflection types) ---
# --------------------------------------------------------------------

set(UNIT_MODULE_NAME "cflex_unit")
set(UNIT_GENERATED_H ${GENERATED_DIR}/${UNIT_MODULE_NAME}_generated.h)
set(UNIT_GENERATED_C ${GENERATED_DIR}/${UNIT_MODULE_NAME}_generated.c)

add_custom_command(
    OUTPUT ${UNIT_GENERATED_C} ${UNIT_GENERATED_H}
    COMMAND $<TARGET_FILE:cflex_build> ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex_unit ${GENERATED_DIR} --name ${UNIT_MODULE_NAME}
    DEPENDS cflex_build
    COMMENT "Generating cflex_unit reflection files"
    VERBATIM
)

# Define a target called "generate_cflx_unit" that depends on these files.
add_custom_target(generate_cflex_unit DEPENDS ${UNIT_GENERATED_C} ${UNIT_GENERATED_H})

# --------------------------------------------------------------------
# Program (example application)
# --------------------------------------------------------------------

# Build the example program "program" from these source files.
add_executable(program
    src/program/program.c
    src/program/program.h
    src/program/program_cflex.c
    ${LIB_HEADER_FILES}
)

# Tell it where to find headers: its own folder, the cflex library, and the generated files.
target_include_directories(program PRIVATE
    src/program
    src/cflex
    ${GENERATED_DIR}
)

# Make sure the reflection files are generated before building "program".
add_dependencies(program generate_cflex_default generate_program)

# Group "program" files under a "program" folder in IDEs.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/program PREFIX "source" 
    FILES src/program/program.c src/program/program.h src/program/program_cflex.c)

# --------------------------------------------------------------------
# CFLX_UNIT (unit testing application)
# --------------------------------------------------------------------

# Build the example program "program" from these source files.
add_executable(cflex_unit
    src/cflex_unit/cflex_unit.c
    src/cflex_unit/cflex_unit_types.h
    src/cflex_unit/cflex_unit_cflex.c
)

# Tell it where to find headers: its own folder, the cflex library, and the generated files.
target_include_directories(cflex_unit PRIVATE
    src/cflex_unit
    src/cflex
    ${GENERATED_DIR}
)

# Again, ensure reflection files exist before compiling.
add_dependencies(cflex_unit generate_cflex_default generate_cflex_unit)

# Group "cflex_unit" files under a "cflex_unit" folder.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex_unit PREFIX "source" 
    FILES src/cflex_unit/cflex_unit.c src/cflex_unit/cflex_unit_types.h src/cflex_unit/cflex_unit_cflex.c)
	
# --------------------------------------------------------------------
