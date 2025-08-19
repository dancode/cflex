#if defined( _WIN32 )
#    define _CRT_SECURE_NO_WARNINGS
#endif

#include "cflex_build.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// Include the internal API header
#include "internal/cflex_internal.h"

// --- Unity Build ---
// Include the implementation files directly.
#include "internal/cflex_std.c"
#include "internal/cflex_platform.c"
#include "internal/cflex_scan.c"
#include "internal/cflex_parse.c"
#include "internal/cflex_output.c"

// --- Global State ---
// These are large data structures, so we make them static globals
// to avoid blowing the stack in main().
static file_list_t   header_files = { 0 };
static parsed_data_t parsed_data  = { 0 };

// -----------------------------------------------------------------------------
// TODO: explain thie build process...
// -----------------------------------------------------------------------------

int
main( int argc, char** argv )
{
    const char* input_path  = NULL;
    const char* output_path = NULL;

    if ( CFLEX_BUILD_DEBUG )
    {
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

    print_fmt( "Cflexbuild Reflection Generator\n" );
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

    find_header_files( input_path, &header_files );
    print_fmt( "Found %d header file(s).\n\n", header_files.count );

    // 2. Parse files
    for ( int i = 0; i < header_files.count; ++i )
    {
        if ( !parse_header_file( header_files.files[ i ], &parsed_data ) )
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
