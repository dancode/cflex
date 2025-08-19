#if defined( _WIN32 )
#    define _CRT_SECURE_NO_WARNINGS
#endif

#include "cflex_build.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

// Include the internal API header
#include "internal/cflex_internal.h"

// --- Unity Build ---
// Include the implementation files directly.
#include "internal/cflex_std.c"
#include "internal/cflex_platform.c"
#include "internal/cflex_scan.c"
#include "internal/cflex_parse_util.c"
#include "internal/cflex_parse_struct.c"
#include "internal/cflex_parse_enum.c"
#include "internal/cflex_parse.c"
#include "internal/cflex_output.c"

// --- Global State ---
// These are large data structures, so we make them static globals
// to avoid blowing the stack in main().
static file_list_t   header_files = { 0 };
static parsed_data_t parsed_data  = { 0 };

// -----------------------------------------------------------------------------
// cflex_build - Reflection Data Generator
// -----------------------------------------------------------------------------
// This tool is a pre-build step for projects using the cflex reflection library.
// It performs the following steps:
//
// 1. Scans a specified input directory for C header files (`.h`).
// 2. Parses these header files to find `struct` and `enum` definitions that
//    are marked up with `CF_` macros (cflex_macros.h).
// 3. Collects information about these types, such as their names, fields,
//    and enum values.
// 4. Generates two files in the specified output directory:
//    - `cflex_generated.h`: A header file containing the generated reflection
//      data structures.
//    - `cflex_generated.c`: A source file containing the implementation for
//      the reflection data.
//
// This generated code can then be compiled and linked into the main
// application, providing runtime access to type information.
// -----------------------------------------------------------------------------

#if CFLEX_BUILD_DEBUG
// Prints sizeof() stats for internal data types and static data structures.
// This provides a quick overview of the memory footprint of the tool's data.
static void
cflex_build_debug_print_stats()
{
    print_fmt( "--- cflex_build debug stats ---\n" );
    print_fmt( "sizeof(file_list_t) = %zu\n", sizeof( file_list_t ) );
    print_fmt( "sizeof(parsed_field_t) = %zu\n", sizeof( parsed_field_t ) );
    print_fmt( "sizeof(parsed_enum_value_t) = %zu\n", sizeof( parsed_enum_value_t ) );
    print_fmt( "sizeof(parsed_type_t) = %zu\n", sizeof( parsed_type_t ) );
    print_fmt( "sizeof(parsed_data_t) = %zu\n", sizeof( parsed_data_t ) );
    print_fmt( "\n" );
    print_fmt( "sizeof(static file_list_t header_files) = %zu\n", sizeof( header_files ) );
    print_fmt( "sizeof(static parsed_data_t parsed_data) = %zu\n", sizeof( parsed_data ) );
    print_fmt( "--- end cflex_build debug stats ---\n\n" );
}
#endif

int
main( int argc, char** argv )
{
    const char* input_path  = NULL;
    const char* output_path = NULL;

    if ( CFLEX_BUILD_DEBUG )
    {
        // When debugging is enabled, this function prints sizeof() stats for
        // internal data types and static data structures.
        cflex_build_debug_print_stats();

        UNUSED( argc );
        UNUSED( argv );
        input_path  = "F:/C/cflex/cflex/src/program";
        output_path = "F:/C/cflex/cflex/build/cflex_generated";
    }
    else
    {
        input_path  = argv[ 1 ];
        output_path = argv[ 2 ];
        if ( argc < 3 )
        {
            if ( 0 )
            {
                file_print_fmt( stderr, "Usage: %s <input_path> <output_path>\n", argv[ 0 ] );
                return 1;
            }
        }
    }

    print_fmt( "CFlex Build Reflection Generator\n" );
    print_fmt( "Input Path: %s\n", input_path );
    print_fmt( "Output Path: %s\n", output_path );

    if ( CFLEX_BUILD_DEBUG )    // debug output when generator runs.
    {
        FILE* f = fopen( "log.txt", "w" );
        if ( !f )
        {
            perror( "fopen" );
            return 1;
        }
        file_print_fmt( f, "Cflexbuild Reflection Generator\n" );
        file_print_fmt( f, "Input Path: %s\n", input_path );
        file_print_fmt( f, "Output Path: %s\n", output_path );
        fclose( f );
    }

    // 1. Scan for files
    print_fmt( "Scanning for header files in %s...\n", input_path );
    scan_for_files( input_path, &header_files );
    print_fmt( "Found %d header file(s).\n\n", header_files.count );

    // 2. Parse files
    for ( int32_t i = 0; i < header_files.count; ++i )
    {
        if ( parse_header_file( header_files.files[ i ], &parsed_data ) == false )
        {
            file_print_fmt( stderr, "Error parsing file, aborting.\n" );
            return 1;
        }
    }

    // 3. Generate output
    if ( !generate_output_files( output_path, &parsed_data, &header_files ) )
    {
        file_print_fmt( stderr, "Error generating output files, aborting.\n" );
        return 1;
    }

    return 0;
}
