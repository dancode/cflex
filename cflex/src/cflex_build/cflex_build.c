#if defined( _WIN32 )
#    define _CRT_SECURE_NO_WARNINGS
#endif

#include "cflex_build.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

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
static file_list_t   header_files = { 0 };
static parsed_data_t parsed_data  = { 0 };

int
main( int argc, char** argv )
{
    const char* input_path         = NULL;
    const char* output_path        = NULL;
    const char* module_name        = "cflex";
    bool        default_types_only = false;

    // --- Argument Parsing ---
    int arg_idx = 1;
    while ( arg_idx < argc )
    {
        const char* arg = argv[ arg_idx ];
        if ( strcmp( arg, "--name" ) == 0 )
        {
            if ( arg_idx + 1 < argc )
            {
                module_name = argv[ arg_idx + 1 ];
                arg_idx += 2;
            }
            else
            {
                file_print_fmt( stderr, "Error: --name requires an argument.\n" );
                return 1;
            }
        }
        else if ( strcmp( arg, "--default-types-only" ) == 0 )
        {
            default_types_only = true;
            arg_idx++;
        }
        else
        {
            if ( !input_path )
            {
                input_path = arg;
            }
            else if ( !output_path )
            {
                output_path = arg;
            }
            arg_idx++;
        }
    }

    if ( 1 )
    {
        input_path  = "F:/C/cflex/cflex/src/program";
        output_path = "F:/C/cflex/cflex/build/cflex_generated";
        module_name = "program";
    }
    else
    {
        if ( !input_path || !output_path )
        {
            file_print_fmt(
                stderr,
                "Usage: %s <input_path> <output_path> [--name <module_name>] [--default-types-only]\n",
                argv[ 0 ] );
            return 1;
        }
    }

    print_fmt( "CFlex Build Reflection Generator\n" );
    print_fmt( "Input Path: %s\n", input_path );
    print_fmt( "Output Path: %s\n", output_path );
    print_fmt( "Module Name: %s\n", module_name );
    if ( default_types_only )
    {
        print_fmt( "Mode: Default Types Only\n" );
    }

    // --- Main Logic ---
    if ( !default_types_only )
    {
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
    }

    // 3. Generate output
    if ( !generate_output_files( output_path, module_name, default_types_only, &parsed_data, &header_files ) )
    {
        file_print_fmt( stderr, "Error generating output files, aborting.\n" );
        return 1;
    }

    return 0;
}