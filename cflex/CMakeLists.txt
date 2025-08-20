cmake_minimum_required(VERSION 3.10)
project(cflex C)

# Configure custom output directories for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")

# Set C11 as the standard, require it, and disable extensions for strictness
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Enable solution folders for better organization in Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add platform-agnostic warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# --- cflex build tool ---
file(GLOB_RECURSE TOOL_SOURCE_FILES "src/cflex_build/*.c" "src/cflex_build/*.h")
add_executable(cflex_build ${TOOL_SOURCE_FILES})
target_include_directories(cflex_build PUBLIC src/cflex_build)
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

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex_build PREFIX "source" FILES ${TOOL_SOURCE_FILES})

# --- Generated Files ---
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/cflex_generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Command to generate default primitive types
set(DEFAULT_MODULE_NAME "cflex_default")
set(DEFAULT_GENERATED_H ${GENERATED_DIR}/${DEFAULT_MODULE_NAME}_generated.h)
set(DEFAULT_GENERATED_C ${GENERATED_DIR}/${DEFAULT_MODULE_NAME}_generated.c)
add_custom_command(
    OUTPUT ${DEFAULT_GENERATED_C} ${DEFAULT_GENERATED_H}
    COMMAND $<TARGET_FILE:cflex_build> ${CMAKE_CURRENT_SOURCE_DIR} ${GENERATED_DIR} --name ${DEFAULT_MODULE_NAME} --default-types-only
    DEPENDS cflex_build
    COMMENT "Generating default reflection files"
    VERBATIM
)
add_custom_target(generate_cflex_default DEPENDS ${DEFAULT_GENERATED_C} ${DEFAULT_GENERATED_H})

# Command to generate 'program' module types
set(PROGRAM_MODULE_NAME "program")
set(PROGRAM_GENERATED_H ${GENERATED_DIR}/${PROGRAM_MODULE_NAME}_generated.h)
set(PROGRAM_GENERATED_C ${GENERATED_DIR}/${PROGRAM_MODULE_NAME}_generated.c)
add_custom_command(
    OUTPUT ${PROGRAM_GENERATED_C} ${PROGRAM_GENERATED_H}
    COMMAND $<TARGET_FILE:cflex_build> ${CMAKE_CURRENT_SOURCE_DIR}/src/program ${GENERATED_DIR} --name ${PROGRAM_MODULE_NAME}
    DEPENDS cflex_build
    COMMENT "Generating program reflection files"
    VERBATIM
)
add_custom_target(generate_program DEPENDS ${PROGRAM_GENERATED_C} ${PROGRAM_GENERATED_H})

# Command to generate 'cflex_unit' module types
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
add_custom_target(generate_cflex_unit DEPENDS ${UNIT_GENERATED_C} ${UNIT_GENERATED_H})


# Command to generate 'cflex_unit' module types
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
add_custom_target(generate_cflex_unit DEPENDS ${UNIT_GENERATED_C} ${UNIT_GENERATED_H})

function(cflex_add_default_module)
    set(MODULE_NAME "cflex_default")
    set(GENERATED_H ${GENERATED_DIR}/${MODULE_NAME}_generated.h)
    set(GENERATED_C ${GENERATED_DIR}/${MODULE_NAME}_generated.c)

    add_custom_command(
        OUTPUT ${GENERATED_C} ${GENERATED_H}
        COMMAND $<TARGET_FILE:cflex_build> ${CMAKE_CURRENT_SOURCE_DIR} ${GENERATED_DIR} --name ${MODULE_NAME} --default-types-only
        DEPENDS cflex_build
        COMMENT "Generating default reflection files for ${MODULE_NAME}"
        VERBATIM
    )

    add_library(${MODULE_NAME}_lib STATIC ${GENERATED_C} ${GENERATED_H})
    target_include_directories(${MODULE_NAME}_lib PUBLIC ${GENERATED_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex ${CMAKE_CURRENT_SOURCE_DIR}/src/${MODULE_NAME})
    add_custom_target(generate_reflection_${MODULE_NAME} DEPENDS ${GENERATED_C} ${GENERATED_H})
    add_dependencies(${MODULE_NAME}_lib generate_reflection_${MODULE_NAME})

    add_library(${MODULE_NAME} INTERFACE)
    target_link_libraries(${MODULE_NAME} INTERFACE ${MODULE_NAME}_lib)
endfunction()

function(cflex_add_module MODULE_NAME)
    set(GENERATED_H ${GENERATED_DIR}/${MODULE_NAME}_generated.h)
    set(GENERATED_C ${GENERATED_DIR}/${MODULE_NAME}_generated.c)

    add_custom_command(
        OUTPUT ${GENERATED_C} ${GENERATED_H}
        COMMAND $<TARGET_FILE:cflex_build> ${CMAKE_CURRENT_SOURCE_DIR}/src/${MODULE_NAME} ${GENERATED_DIR} --name ${MODULE_NAME}
        DEPENDS cflex_build
        COMMENT "Generating reflection files for ${MODULE_NAME}"
        VERBATIM
    )

    add_library(${MODULE_NAME}_lib STATIC ${GENERATED_C} ${GENERATED_H})
    target_include_directories(${MODULE_NAME}_lib PUBLIC ${GENERATED_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex ${CMAKE_CURRENT_SOURCE_DIR}/src/${MODULE_NAME})
    add_custom_target(generate_reflection_${MODULE_NAME} DEPENDS ${GENERATED_C} ${GENERATED_H})
    add_dependencies(${MODULE_NAME}_lib generate_reflection_${MODULE_NAME})

    add_library(${MODULE_NAME}_module INTERFACE)
    target_link_libraries(${MODULE_NAME}_module INTERFACE ${MODULE_NAME}_lib)
endfunction()

# --- Module Definitions ---
cflex_add_default_module()
cflex_add_module(program)
cflex_add_module(cflex_unit)

# --- Example Application ---

add_executable(program
    src/program/program.c
    src/program/program.h
    src/program/program_cflex.c
)
target_include_directories(program PRIVATE
    src/program
    src/cflex
    ${GENERATED_DIR}
)
add_dependencies(program generate_cflex_default generate_program)

# --- Unit Test Application ---
add_executable(cflex_unit
    src/cflex_unit/cflex_unit.c
    src/cflex_unit/cflex_unit_types.h
    src/cflex_unit/cflex_unit_cflex.c
)
target_include_directories(cflex_unit PRIVATE
    src/cflex_unit
    src/cflex
    ${GENERATED_DIR}
)
add_dependencies(cflex_unit generate_cflex_default generate_cflex_unit)

# --- IDE File Organization ---
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/program PREFIX "program" FILES src/program/program.c src/program/program.h src/program/program_cflex.c)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex_unit PREFIX "cflex_unit" FILES src/cflex_unit/cflex_unit.c src/cflex_unit/cflex_unit_types.h src/cflex_unit/cflex_unit_cflex.c)
file(GLOB_RECURSE LIB_HEADER_FILES "src/cflex/*.h")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex PREFIX "cflex" FILES ${LIB_HEADER_FILES})

