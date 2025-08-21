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

# Set the RPATH so that executables can find shared libraries in the same directory.
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)

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
# FUNCTION: add_cflex_target
#
# This function automates the process of creating a target (executable or library)
# that uses the cflex reflection system. It handles:
#   1. Setting up custom commands to run the cflex_build tool.
#   2. Passing the correct flags based on target type (executables get default types).
#   3. Creating the target itself (add_executable or add_library).
#   4. Linking dependencies and include directories.
#   5. Organizing source files in the IDE.
#
# Usage:
#   add_cflex_target(<name> <type> [source1] [source2] ...)
#     <name>: The name of the target.
#     <type>: EXECUTABLE or LIBRARY.
#
function(add_cflex_target target_name type)
    set(options)
    set(one_value_args)
    set(multi_value_args SOURCES)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    # --- Generated File Setup ---
    set(MODULE_NAME ${target_name})
    set(GENERATED_H ${GENERATED_DIR}/${MODULE_NAME}_generated.h)
    set(GENERATED_C ${GENERATED_DIR}/${MODULE_NAME}_generated.c)

    # --- Code Generation Command ---
    set(CFLEX_COMMAND $<TARGET_FILE:cflex_build> ${CMAKE_CURRENT_SOURCE_DIR}/src/${MODULE_NAME} ${GENERATED_DIR} --name ${MODULE_NAME})
    if(type STREQUAL "EXECUTABLE")
        list(APPEND CFLEX_COMMAND --include-default-types)
    endif()

    add_custom_command(
        OUTPUT ${GENERATED_C} ${GENERATED_H}
        COMMAND ${CFLEX_COMMAND}
        DEPENDS cflex_build
        COMMENT "Generating ${MODULE_NAME} reflection files"
        VERBATIM
    )

    set(GENERATE_TARGET "generate_${MODULE_NAME}")
    add_custom_target(${GENERATE_TARGET} DEPENDS ${GENERATED_C} ${GENERATED_H})
    set_target_properties(${GENERATE_TARGET} PROPERTIES FOLDER "CMakePredefinedTargets/Hidden")

    # --- Target Creation ---
    if(type STREQUAL "EXECUTABLE")
        add_executable(${target_name} ${ARG_SOURCES})
    elseif(type STREQUAL "LIBRARY")
        add_library(${target_name} ${ARG_SOURCES})
    endif()

    # --- Dependencies and Includes ---
    target_include_directories(${target_name} PRIVATE
        src/${target_name}
        src/cflex
        ${GENERATED_DIR}
    )

    target_sources(${target_name} PRIVATE ${GENERATED_C} ${GENERATED_H})
    set_source_files_properties(${GENERATED_C} ${GENERATED_H} PROPERTIES HEADER_FILE_ONLY ON)

    add_dependencies(${target_name} ${GENERATE_TARGET})

    # --- IDE Organization ---
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/${target_name} PREFIX "source" FILES ${ARG_SOURCES})
    source_group(TREE ${GENERATED_DIR} PREFIX "generated" FILES ${GENERATED_C} ${GENERATED_H})

endfunction()

# --------------------------------------------------------------------
# FUNCTION: add_cflex_module_target
#
# This function is a variant of add_cflex_target specifically for modules
# located in subdirectories (e.g., project/modules/module_name).
#
function(add_cflex_module_target project_name module_name type)
    set(options)
    set(one_value_args)
    set(multi_value_args SOURCES)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    # --- Generated File Setup ---
    set(GENERATED_H ${GENERATED_DIR}/${project_name}_${module_name}_generated.h)
    set(GENERATED_C ${GENERATED_DIR}/${project_name}_${module_name}_generated.c)
    set(MODULE_PATH "src/${project_name}/modules/${module_name}")

    # --- Code Generation Command ---
    set(CFLEX_COMMAND $<TARGET_FILE:cflex_build> ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_PATH} ${GENERATED_DIR} --name ${project_name}_${module_name})
    if(type STREQUAL "EXECUTABLE")
        list(APPEND CFLEX_COMMAND --include-default-types)
    endif()

    add_custom_command(
        OUTPUT ${GENERATED_C} ${GENERATED_H}
        COMMAND ${CFLEX_COMMAND}
        DEPENDS cflex_build
        COMMENT "Generating ${project_name}_${module_name} reflection files"
        VERBATIM
    )

    set(GENERATE_TARGET "generate_${project_name}_${module_name}")
    add_custom_target(${GENERATE_TARGET} DEPENDS ${GENERATED_C} ${GENERATED_H})
    set_target_properties(${GENERATE_TARGET} PROPERTIES FOLDER "CMakePredefinedTargets/Hidden")

    # --- Target Creation ---
    set(TARGET_NAME "${project_name}_${module_name}")
    if(type STREQUAL "EXECUTABLE")
        add_executable(${TARGET_NAME} ${ARG_SOURCES})
    elseif(type STREQUAL "LIBRARY")
        add_library(${TARGET_NAME} ${ARG_SOURCES})
    elseif(type STREQUAL "SHARED_LIBRARY")
        add_library(${TARGET_NAME} SHARED ${ARG_SOURCES})
    endif()

    # --- Dependencies and Includes ---
    target_include_directories(${TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_PATH}
        src/cflex
        ${GENERATED_DIR}
    )

    target_sources(${TARGET_NAME} PRIVATE ${GENERATED_C} ${GENERATED_H})

    add_dependencies(${TARGET_NAME} ${GENERATE_TARGET})

    # --- IDE Organization ---
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_PATH} PREFIX "source" FILES ${ARG_SOURCES})
    source_group(TREE ${GENERATED_DIR} PREFIX "generated" FILES ${GENERATED_C} ${GENERATED_H})

endfunction()

# --------------------------------------------------------------------
# --- CFLEX LIbrary (Header-Only)
# --------------------------------------------------------------------

# Create an INTERFACE library for cflex. This is the modern CMake way to handle
# header-only libraries, as it allows usage requirements (like include paths)
# to be attached to the library and propagated to consumers.
# add_library(cflex INTERFACE)

# The cflex library needs its own source directory for its header, and crucially,
# it also depends on the generated files. Adding the generated directory to the
# INTERFACE ensures that any target linking against cflex gets this path,
# which is the most robust way to solve IDE IntelliSense issues.
# target_include_directories(cflex INTERFACE
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex
#     # ${GENERATED_DIR}
# )


# --------------------------------------------------------------------
# Program (example application)
# --------------------------------------------------------------------
add_cflex_target(program EXECUTABLE
    SOURCES
        src/program/program.c
        src/program/program.h
        src/program/program_cflex.c
)

# --------------------------------------------------------------------
# CFLX_UNIT (unit testing application)
# --------------------------------------------------------------------
add_cflex_target(cflex_unit EXECUTABLE
    SOURCES
        src/cflex_unit/cflex_unit.c
        src/cflex_unit/cflex_unit_types.h
        src/cflex_unit/cflex_unit_cflex.c
)

# --------------------------------------------------------------------
# EXAMPLE_LIB (static library example)
# --------------------------------------------------------------------
add_cflex_module_target(example_lib module_1 LIBRARY
    SOURCES
        src/example_lib/modules/module_1/module_1.c
        src/example_lib/modules/module_1/module_1.h
)

add_cflex_target(example_lib EXECUTABLE
    SOURCES
        src/example_lib/example_lib.c
        src/example_lib/example_lib.h
        src/example_lib/example_lib_cflex.c
)

target_link_libraries(example_lib PRIVATE example_lib_module_1)

# --------------------------------------------------------------------
# EXAMPLE_DLL (dynamic library example)
# --------------------------------------------------------------------
set(BUILD_SHARED_LIBS ON)

add_cflex_module_target(example_dll module_1 SHARED_LIBRARY
    SOURCES
        src/example_dll/modules/module_1/module_1.c
        src/example_dll/modules/module_1/module_1.h
)

add_cflex_module_target(example_dll module_2 SHARED_LIBRARY
    SOURCES
        src/example_dll/modules/module_2/module_2.c
        src/example_dll/modules/module_2/module_2.h
)

add_cflex_target(example_dll EXECUTABLE
    SOURCES
        src/example_dll/example_dll.c
        src/example_dll/example_dll.h
        src/example_dll/example_dll_cflex.c
)

if(UNIX)
    target_link_libraries(example_dll PRIVATE dl)
endif()


# --------------------------------------------------------------------

set_target_properties(program PROPERTIES FOLDER "Apps")
set_target_properties(cflex_unit PROPERTIES FOLDER "Tests")
set_target_properties(cflex_build PROPERTIES FOLDER "Tools")
set_target_properties(example_lib PROPERTIES FOLDER "Examples/Static")
set_target_properties(example_lib_module_1 PROPERTIES FOLDER "Examples/Static")
set_target_properties(example_dll PROPERTIES FOLDER "Examples/Dynamic")
set_target_properties(example_dll_module_1 PROPERTIES FOLDER "Examples/Dynamic")
set_target_properties(example_dll_module_2 PROPERTIES FOLDER "Examples/Dynamic")