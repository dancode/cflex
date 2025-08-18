#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Fixes path separators for cross-platform compatibility.
void
fix_path_separators( char* path )
{
    if ( path == NULL )
    {
        return;
    }

    size_t length = strlen( path );
    if ( length == 0 )
    {
        return;
    }

    // Iterate through the string and replace the 'other' separator with the 'os' separator.
    for ( size_t i = 0; i < length; ++i )
    {
        if ( path[ i ] == PATH_CHAR_SEPARATOR_WRONG )
        {
            path[ i ] = PATH_CHAR_SEPARATOR;
        }
    }
}

// Simple helper to check if a string ends with a suffix.
static bool
ends_with( const char* str, const char* suffix )
{
    if ( !str || !suffix )
    {
        return false;
    }
    size_t str_len    = strlen( str );
    size_t suffix_len = strlen( suffix );
    if ( suffix_len > str_len )
    {
        return false;
    }
    return strncmp( str + str_len - suffix_len, suffix, suffix_len ) == 0;
}

static file_list_t all_files;

void
find_header_files( const char* path, file_list_t* header_files )
{
    if ( !scan_directory( path, &all_files ) )
    {
        header_files->count = 0;
        return;
    }

    header_files->count = 0;
    for ( int i = 0; i < all_files.count; ++i )
    {
        if ( ends_with( all_files.files[ i ], ".h" ) )
        {
            if ( header_files->count < MAX_FILES )
            {
                str_copy( header_files->files[ header_files->count ], all_files.files[ i ], MAX_PATH_LENGTH );
                fix_path_separators( header_files->files[ header_files->count ] );
                header_files->count++;
            }
            else
            {
                fprintf( stderr, "Warning: Exceeded max header file limit of %d\n", MAX_FILES );
                break;
            }
        }
    }
}
