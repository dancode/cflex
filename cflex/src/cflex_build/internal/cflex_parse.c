/*==============================================================================================

    Parse Header

    This file contains the core logic for parsing C header files to extract
    reflection data. The parser is a simple, hand-written recursive descent
    parser. It operates on a single file's content loaded into a memory buffer.
    It is not a full C parser; it only looks for specific patterns (`CF_STRUCT()`,
    `CF_ENUM()`) and parses the `typedef struct` and `typedef enum` that follow.

==============================================================================================*/

// A static buffer to hold the entire content of a header file.
// Using a static variable avoids allocating a large buffer on the stack.

#define FILE_BUFFER_SIZE ( 1024 * 1024 )
static char file_buffer[ FILE_BUFFER_SIZE ];

/*============================================================================================*/

// Parses a single header file for reflection data.
// It reads the entire file into a buffer, then scans for `CF_STRUCT` and
// `CF_ENUM` annotations, dispatching to the appropriate parser.

static bool
parse_header_file( const char* filepath, parsed_data_t* data )
{
    // Read the entire file into the static file_buffer.

    FILE* fp = fopen( filepath, "r" );
    if ( !fp )
    {
        file_print_fmt( stderr, "Error: Could not open file %s\n", filepath );
        return false;
    }

    size_t bytes_read         = fread( file_buffer, 1, FILE_BUFFER_SIZE - 1, fp );
    file_buffer[ bytes_read ] = '\0';
    fclose( fp );

    const char* cursor = file_buffer;
    const char* end    = file_buffer + bytes_read;

    while ( *cursor )
    {
        // Find the next 'C' using memchr to skip irrelevant characters
        size_t      remaining = (size_t)( end - cursor );
        const char* next_c    = memchr( cursor, 'C', remaining );
        if ( next_c == NULL )
        {
            break;    // No more annotations
        }

        cursor = next_c;

        // Quick first-three-character check (for optimization to avoid str_ncmp)
        if ( cursor + 2 < end && cursor[ 1 ] == 'F' && cursor[ 2 ] == '_' )
        {
            const char* suffix = cursor + 3;

            // Hard coded branch less comparison.
            bool is_struct = suffix[ 0 ] == 'S' && suffix[ 1 ] == 'T' && suffix[ 2 ] == 'R' &&
                             suffix[ 3 ] == 'U' && suffix[ 4 ] == 'C' && suffix[ 5 ] == 'T' &&
                             suffix[ 6 ] == '(' && suffix[ 7 ] == ')';

            bool is_enum = suffix[ 0 ] == 'E' && suffix[ 1 ] == 'N' && suffix[ 2 ] == 'U' &&
                           suffix[ 3 ] == 'M' && suffix[ 4 ] == '(' && suffix[ 5 ] == ')';

            // Function pointer to the correct parser (or NULL if no match)
            // (branchless parser selection using a single function pointer assignment)
            const char* ( *parser )( const char*, parsed_data_t* ) = is_struct ? parse_struct
                                                                     : is_enum ? parse_enum
                                                                               : NULL;
            if ( parser == NULL )
            {
                cursor += 3;    // If "CF_" but not a recognized tag, skip past it.
                continue;
            }

            int32_t advance = is_struct ? 8 : 6;    // length of "STRUCT()" or "ENUM()"
            cursor          = parser( suffix + advance, data );
            if ( !cursor )
            {
                file_print_fmt( stderr, "Error: parsing file %s\n", filepath );
                return false;
            }
        }
        else
        {
            cursor++;    // Not "CF_", skip one char
        }
    }

    return true;
}

/*============================================================================================*/
