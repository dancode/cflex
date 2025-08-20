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
set_target_properties(generate_cflex_default PROPERTIES FOLDER "CMakePredefinedTargets/Hidden")

# --------------------------------------------------------------------
# --- Program Module (reflection types) ---
# --------------------------------------------------------------------

set(PROGRAM_MODULE_NAME "program")
set(PROGRAM_GENERATED_H ${GENERATED_DIR}/${PROGRAM_MODULE_NAME}_generated.h)
set(PROGRAM_GENERATED_C ${GENERATED_DIR}/${PROGRAM_MODULE_NAME}_generated.c)

# Tell CMake how to create these files.
# The "cflex_build" tool is run to generate them (with the command arugments specified)
# It will output is older than depdency, it will generate again (build tool was updated)

add_custom_command(
    OUTPUT ${PROGRAM_GENERATED_C} ${PROGRAM_GENERATED_H} #we want these files!
    COMMAND $<TARGET_FILE:cflex_build> ${CMAKE_CURRENT_SOURCE_DIR}/src/program ${GENERATED_DIR} --name ${PROGRAM_MODULE_NAME}
    DEPENDS cflex_build #build me first!
    COMMENT "Generating program reflection files"
    VERBATIM
)

# note: This command will force rebuild every compile
# COMMAND ${CMAKE_COMMAND} -E rm -f ${PROGRAM_GENERATED_C} ${PROGRAM_GENERATED_H}

# Define a target called "generate_program" that depends on these files.
add_custom_target(generate_program DEPENDS ${PROGRAM_GENERATED_C} ${PROGRAM_GENERATED_H})
set_target_properties(generate_program PROPERTIES FOLDER "CMakePredefinedTargets/Hidden")

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
set_target_properties(generate_cflex_unit PROPERTIES FOLDER "CMakePredefinedTargets/Hidden")

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

# **NEW** Add the generated headers/sources to the program target explicitly 
# (important for IntelliSense & IDEs)
target_sources(program PRIVATE ${PROGRAM_GENERATED_C} ${PROGRAM_GENERATED_H})

# Link the program against the cflex library. This transitively applies the
# include directories from cflex to the program, which is a more robust way
# of managing dependencies for IDEs and build systems.
# target_link_libraries(program PRIVATE cflex)

# Make sure the reflection files are generated before building "program".
add_dependencies(program generate_cflex_default generate_program)

# Group "program" files under a "program" folder in IDEs.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/program PREFIX "source" 
    FILES src/program/program.c src/program/program.h src/program/program_cflex.c)

# Group "program" files under a "program" folder in IDEs.
source_group(TREE ${GENERATED_DIR} PREFIX "generated" 
    FILES ${PROGRAM_GENERATED_C} ${PROGRAM_GENERATED_H} )


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

# **NEW** Add the generated headers/sources to the program target explicitly 
# (important for IntelliSense & IDEs)
target_sources(cflex_unit PRIVATE ${UNIT_GENERATED_C} ${UNIT_GENERATED_H})

# Link the program against the cflex library. This transitively applies the
# include directories from cflex to the program, which is a more robust way
# of managing dependencies for IDEs and build systems.
target_link_libraries(cflex_unit PRIVATE cflex)

# Again, ensure reflection files exist before compiling.
add_dependencies(cflex_unit generate_cflex_default generate_cflex_unit)

# Group "cflex_unit" files under a "cflex_unit" folder.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex_unit PREFIX "source" 
    FILES src/cflex_unit/cflex_unit.c src/cflex_unit/cflex_unit_types.h src/cflex_unit/cflex_unit_cflex.c)

# Group "program" files under a "program" folder in IDEs.
source_group(TREE ${GENERATED_DIR} PREFIX "generated" 
    FILES ${UNIT_GENERATED_C} ${UNIT_GENERATED_H} )
	
# --------------------------------------------------------------------

set_target_properties(program PROPERTIES FOLDER "Apps")
set_target_properties(cflex_unit PROPERTIES FOLDER "Tests")
set_target_properties(cflex_build PROPERTIES FOLDER "Tools")

# --- Hide CMake's predefined/utility targets in Visual Studio ---
# set(CMAKE_SUPERFLOUS_TARGETS ALL_BUILD INSTALL PACKAGE RUN_TESTS ZERO_CHECK )
# foreach(tgt IN LISTS CMAKE_SUPERFLOUS_TARGETS)
#     if(TARGET ${tgt})
#         set_target_properties(${tgt} PROPERTIES FOLDER "CMakePredefinedTargets/Hidden")
#     endif()
# endforeach()

# set_target_properties(ALL_BUILD PROPERTIES FOLDER "CMakePredefinedTargets/Hidden")
# set_target_properties(INSTALL   PROPERTIES FOLDER "CMakePredefinedTargets/Hidden")
# set_target_properties(PACKAGE   PROPERTIES FOLDER "CMakePredefinedTargets/Hidden")
# set_target_properties(RUN_TESTS PROPERTIES FOLDER "CMakePredefinedTargets/Hidden")
# set_target_properties(ZERO_CHECK PROPERTIES FOLDER "CMakePredefinedTargets/Hidden")
