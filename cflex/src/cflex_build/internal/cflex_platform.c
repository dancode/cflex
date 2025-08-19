#ifdef _WIN32
#    define NOMINMAX
#    define WIN32_LEAN_AND_MEAN 1
#    define WIN32_EXTRA_LEAN    1
#    define VC_EXTRALEAN        1
#    include <windows.h>
#else
#    include <dirent.h>
#    include <sys/stat.h>
#endif

// Scans a directory for all files and subdirectories, populating the file_list.
// This function is implemented with platform-specific code for Windows and POSIX.
static bool
platform_scan_directory( const char* path, file_list_t* file_list )
{
    file_list->count = 0;

#ifdef _WIN32

    char search_path[ MAX_PATH_LENGTH ];
    str_print_fmt( search_path, sizeof( search_path ), "%s\\*", path );

    WIN32_FIND_DATAA find_data;
    HANDLE           h_find = FindFirstFileA( search_path, &find_data );

    if ( h_find == INVALID_HANDLE_VALUE )
    {
        file_print_fmt( stderr, "Error: Could not open directory %s\n", path );
        return false;
    }

    do {
        if ( str_cmp( find_data.cFileName, "." ) != 0 && str_cmp( find_data.cFileName, ".." ) != 0 )
        {
            if ( file_list->count < MAX_FILES )
            {
                str_print_fmt( file_list->files[ file_list->count ], MAX_PATH_LENGTH, "%s\\%s", path,
                               find_data.cFileName );
                file_list->count++;
            }
            else
            {
                file_print_fmt( stderr, "Warning: Exceeded max file limit of %d\n", MAX_FILES );
                break;
            }
        }
    }
    while ( FindNextFileA( h_find, &find_data ) != 0 );

    FindClose( h_find );

#else    // POSIX

    DIR* dir = opendir( path );
    if ( dir == NULL )
    {
        print_fprintf( stderr, "Error: Could not open directory %s\n", path );
        return false;
    }

    struct dirent* entry;
    while ( ( entry = readdir( dir ) ) != NULL )
    {
        if ( str_cmp( entry->d_name, "." ) != 0 && str_cmp( entry->d_name, ".." ) != 0 )
        {
            if ( file_list->count < MAX_FILES )
            {
                print_snprintf( file_list->files[ file_list->count ], MAX_PATH_LENGTH, "%s/%s", path,
                                entry->d_name );
                file_list->count++;
            }
            else
            {
                print_fprintf( stderr, "Warning: Exceeded max file limit of %d\n", MAX_FILES );
                break;
            }
        }
    }

    closedir( dir );

#endif

    return true;
}
