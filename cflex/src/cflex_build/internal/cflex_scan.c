/*==============================================================================================

    File Scanner

==============================================================================================*/

// Ensures that the path separators in a file path are correct for the current OS.
// For example, it will replace all '/' with '\' on Windows.

static void
path_fix_separators( char* path )
{
    if ( path == NULL )
    {
        return;
    }

    int32_t length = str_len( path );
    if ( length == 0 )
    {
        return;
    }

    // Iterate through the string and replace the 'other' separator with the 'os' separator.
    for ( int32_t i = 0; i < length; ++i )
    {
        if ( path[ i ] == PATH_CHAR_SEPARATOR_WRONG )
        {
            path[ i ] = PATH_CHAR_SEPARATOR;
        }
    }
}

/*============================================================================================*/

// A temporary, static buffer to hold all files found in the initial directory scan.
// Using a static variable avoids allocating a large structure on the stack.

static file_list_t all_files = { 0 };

/*============================================================================================*/

// Scans the given path for all files, then filters them to find only files
// ending with the ".h" extension. The results are stored in `header_files`.

static void
scan_for_files( const char* path, file_list_t* header_files )
{
    if ( !platform_scan_directory( path, &all_files ) )
    {
        header_files->count = 0;
        return;
    }

    header_files->count = 0;
    for ( int i = 0; i < all_files.count; ++i )
    {
        if ( str_ends_with( all_files.files[ i ], ".h" ) )
        {
            if ( header_files->count < MAX_FILES )
            {
                str_copy( header_files->files[ header_files->count ], all_files.files[ i ], MAX_PATH_LENGTH );
                path_fix_separators( header_files->files[ header_files->count ] );
                header_files->count++;
            }
            else
            {
                file_print_fmt( stderr, "Warning: Exceeded max header file limit of %d\n", MAX_FILES );
                break;
            }
        }
    }
}

/*============================================================================================*/