#if defined( _WIN32 )
#    define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>

// Include the internal API header
#include "internal/cflex_internal.h"

// --- Unity Build ---
// Include the implementation files directly.
#include "internal/cflex_platform.c"
#include "internal/cflex_string.c"
#include "internal/cflex_scan.c"
#include "internal/cflex_parse.c"
#include "internal/cflex_output.c"

// --- Global State ---
// These are large data structures, so we make them static globals
// to avoid blowing the stack in main().
static file_list_t   header_files = { 0 };
static parsed_data_t parsed_data  = { 0 };

#include <direct.h>

#define CFLEX_BUILD_DEBUG    // define to allow runnig build tool to debug directly without using command line

int
main( int argc, char** argv )
{
    // These paths start with hard coded values for debugging standalone builds.
    const char* input_path  = "F:/C/cflex/cflex/src/program";
    const char* output_path = "F:/C/cflex/cflex/build/cflex_generated";

    // this macro is
#ifndef CFLEX_BUILD_DEBUG
    if ( argc < 3 )
    {
        if ( 0 )
        {
            fprintf( stderr, "Usage: %s <input_path> <output_path>\n", argv[ 0 ] );
            return 1;
        }
    }
#else
    (void)argc;
#endif

    // proper command input if command line is used.
    if ( argc >= 3 )
    {
        input_path  = argv[ 1 ];
        output_path = argv[ 2 ];
    }

    printf( "Cflexbuild Reflection Generator\n" );
    printf( "Input Path: %s\n", input_path );
    printf( "Output Path: %s\n", output_path );

    if ( 1 )
    {
        FILE* f = fopen( "log.txt", "w" );
        if ( !f )
        {
            perror( "fopen" );
            return 1;
        }

        char buf[ 1024 ];
        if ( _getcwd( buf, sizeof( buf ) ) )
        {
            fprintf( f, "Current working directory: %s\n", buf );
        }

        fprintf( f, "Cflexbuild Reflection Generator\n" );
        fprintf( f, "Input Path: %s\n", input_path );
        fprintf( f, "Output Path: %s\n", output_path );

        fclose( f );
    }

    // 1. Scan for files
    printf( "Scanning for header files in %s...\n", input_path );

    find_header_files( input_path, &header_files );
    printf( "Found %d header file(s).\n\n", header_files.count );

    // 2. Parse files
    for ( int i = 0; i < header_files.count; ++i )
    {
        if ( !parse_header_file( header_files.files[ i ], &parsed_data ) )
        {
            fprintf( stderr, "Error parsing file, aborting.\n" );
            return 1;
        }
    }

    // 3. Generate output
    if ( !generate_output_files( output_path, &parsed_data, &header_files ) )
    {
        fprintf( stderr, "Error generating output files, aborting.\n" );
        return 1;
    }

    return 0;
}
