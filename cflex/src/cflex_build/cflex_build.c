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

int
main( int argc, char** argv )
{
    if ( argc < 3 )
    {
        fprintf( stderr, "Usage: %s <input_path> <output_path>\n", argv[ 0 ] );
        return 1;
    }

    const char* input_path = argv[ 1 ];
    const char* output_path = argv[ 2 ];

    printf( "Cflexbuild Reflection Generator\n" );
    printf( "Input Path: %s\n", input_path );
    printf( "Output Path: %s\n", output_path );

    // 1. Scan for files
    printf("Scanning for header files in %s...\n", input_path);

    find_header_files(input_path, &header_files);
    printf("Found %d header file(s).\n\n", header_files.count);

    // 2. Parse files

    for (int i = 0; i < header_files.count; ++i) {
        if (!parse_header_file(header_files.files[i], &parsed_data)) {
            fprintf(stderr, "Error parsing file, aborting.\n");
            return 1;
        }
    }

    // 3. Generate output
    if (!generate_output_files(output_path, &parsed_data, &header_files)) {
        fprintf(stderr, "Error generating output files, aborting.\n");
        return 1;
    }

    return 0;
}
