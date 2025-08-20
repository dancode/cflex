cmake_minimum_required(VERSION 3.10)
project(cflex C)

# Configure custom output directories for binaries
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

# --- cflex library (header-only) ---
add_library(cflex INTERFACE)
target_include_directories(cflex INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex)

# --- cflex helper functions ---
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/cflex_generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

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
file(GLOB_RECURSE PROGRAM_SOURCE_FILES "src/program/*.c" "src/program/*.h")
add_executable(program ${PROGRAM_SOURCE_FILES})
target_link_libraries(program PRIVATE cflex cflex_default program_module)

# --- Unit Test Application ---
add_executable(cflex_unit src/cflex_unit/cflex_unit.c)
target_include_directories(cflex_unit PRIVATE src/program)
target_link_libraries(cflex_unit PRIVATE cflex cflex_default program_module cflex_unit_module)

# --- IDE File Organization ---
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/program PREFIX "program" FILES ${PROGRAM_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex_unit PREFIX "cflex_unit" FILES src/cflex_unit/cflex_unit.c)
file(GLOB_RECURSE LIB_HEADER_FILES "src/cflex/*.h")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/cflex PREFIX "cflex" FILES ${LIB_HEADER_FILES})
