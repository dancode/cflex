#ifndef CFLEX_INTERNAL_H
#define CFLEX_INTERNAL_H

/*==============================================================================================

    * This is the single internal header for the cflex_build tool.
    * It contains forward declarations for all functions and data structures.
    * It is used across the different internal modules.

==============================================================================================*/

// A macro to suppress unused variable warnings.
#define UNUSED( x ) ( (void)x )

/*==============================================================================================

    Platform

==============================================================================================*/

#define MAX_FILES       256
#define MAX_PATH_LENGTH 512

// Holds a list of file paths.
typedef struct file_list_t
{
    char    files[ MAX_FILES ][ MAX_PATH_LENGTH ];
    int32_t count;

} file_list_t;

// Scans a directory for files and populates the file_list.
// Returns false on failure.

static bool platform_scan_directory( const char* path, file_list_t* file_list );

/*==============================================================================================

    Scan

==============================================================================================*/

// Check if compiling for a Windows environment.
#if defined( _WIN32 )
const char PATH_CHAR_SEPARATOR       = '\\';
const char PATH_CHAR_SEPARATOR_WRONG = '/';
#else
// Assumes a Unix-like system (Linux, macOS, etc.) otherwise.
const char PATH_CHAR_SEPARATOR       = '/';
const char PATH_CHAR_SEPARATOR_WRONG = '\\';
#endif

// Finds all files with a .h extension in a directory and populates the header_files list.
static void scan_for_files( const char* path, file_list_t* header_files );

/*==============================================================================================

    Parse

==============================================================================================*/

#define MAX_NAME_LENGTH 256
#define MAX_FIELDS      64
#define MAX_ENUM_VALUES 128
#define MAX_USER_TYPES  128

// Represents a single field within a parsed struct.
typedef struct parsed_field_t
{
    char type_name[ MAX_NAME_LENGTH ];
    char name[ MAX_NAME_LENGTH ];
} parsed_field_t;

// Represents a single value within a parsed enum.
typedef struct parsed_enum_value_t
{
    char name[ MAX_NAME_LENGTH ];
} parsed_enum_value_t;

// Discriminator for the parsed_type_t union.
typedef enum parsed_kind_t
{
    PARSED_KIND_STRUCT,
    PARSED_KIND_ENUM
} parsed_kind_t;

// Represents a single parsed type (either a struct or an enum).
// This is a tagged union, with `kind` as the discriminator.
typedef struct parsed_type_t
{
    parsed_kind_t kind;
    char          name[ MAX_NAME_LENGTH ];
    union
    {
        // Information specific to structs.
        struct
        {
            parsed_field_t fields[ MAX_FIELDS ];
            int            num_fields;
        } struct_info;

        // Information specific to enums.
        struct
        {
            parsed_enum_value_t values[ MAX_ENUM_VALUES ];
            int                 num_values;
        } enum_info;
    };

} parsed_type_t;

// A structure to hold all parsed reflection data from all processed files.
typedef struct parsed_data_t
{
    parsed_type_t types[ MAX_USER_TYPES ];
    int           num_types;

} parsed_data_t;

// Parses a single header file and adds its reflection data to the parsed_data object.
// Returns false on failure.
static bool parse_header_file( const char* filepath, parsed_data_t* data );

/*==============================================================================================

    Output

==============================================================================================*/

// Generates the cflex_generated.h and cflex_generated.c files.
// Returns false on failure.
bool generate_output_files( const char*      output_path,
                            const char*      module_name,
                            bool             default_types_only,
                            const parsed_data_t* data,
                            const file_list_t*   headers );

/*==============================================================================================

    Standard (STD Wrappers)

==============================================================================================*/

// character

int  char_is_space( int c );
int  char_is_alnum( int c );
int  char_is_alpha( int c );
int  char_is_digit( int c );
int  char_to_upper( int c );
bool char_is_identifier( char c );

// printing

int file_print_fmt( FILE* stream, const char* format, ... );
int print_fmt( const char* format, ... );
int str_print_fmt( char* str, int32_t size, const char* format, ... );

// memory

void* mem_alloc( int32_t size );
void* mem_calloc( int32_t num, int32_t size );
void  mem_free( void* ptr );
void* mem_copy( void* dest, const void* src, int32_t n );
void* mem_set( void* s, int c, int32_t n );

// string

int32_t str_cmp( const char* s1, const char* s2 );
int32_t str_ncmp( const char* s1, const char* s2, int32_t n );
int32_t str_len( const char* s );
char*   str_cpy( char* dest, const char* src );
char*   str_ncpy( char* dest, const char* src, int32_t n );
char*   str_cat( char* dest, const char* src );
char*   str_ncat( char* dest, const char* src, int32_t n );
char*   str_chr( const char* s, int c );
char*   str_rchr( const char* s, int c );
char*   str_str( const char* haystack, const char* needle );

void        str_copy( char* dst, const char* src, int32_t dst_size );
void        str_copy_sub( char* dst, const char* src, int32_t len, int32_t dst_size );
const char* str_left_trim( const char* str );
bool        str_ends_with( const char* str, const char* suffix );

/*============================================================================================*/
#endif    // CFLEX_INTERNAL_H
