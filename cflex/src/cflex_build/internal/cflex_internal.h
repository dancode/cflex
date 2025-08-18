#ifndef CFLEX_INTERNAL_H
#define CFLEX_INTERNAL_H

// This is the single internal header for the cflex_build tool.
// It contains forward declarations for all functions and data structures
// used across the different internal modules.

#include <stdbool.h>
#include <stdint.h>

// -----------------------------------------------------------------------------
// Platform
// -----------------------------------------------------------------------------
#define MAX_FILES 1024
#define MAX_PATH_LENGTH 1024

typedef struct file_list_t {
    char files[MAX_FILES][MAX_PATH_LENGTH];
    int count;
} file_list_t;

// Scans a directory for files and populates the file_list.
// Returns false on failure.
bool scan_directory(const char* path, file_list_t* file_list);

// -----------------------------------------------------------------------------
// String
// -----------------------------------------------------------------------------

// Returns a pointer to the first non-whitespace character in the string.
const char* ltrim(const char* str);

// Checks if a character can be part of a C identifier.
bool is_identifier_char(char c);

// Safely copies a null-terminated string. Always null-terminates the destination.
void str_copy(char* dst, const char* src, int32_t dst_size);

// Safely copies N characters from a source string. Always null-terminates.
void substr_copy(char* dst, const char* src, int32_t len, int32_t dst_size);

// -----------------------------------------------------------------------------
// Scan
// -----------------------------------------------------------------------------

// Check if compiling for a Windows environment.
// _WIN32 is a common preprocessor macro defined by most compilers for Windows.
#if defined( _WIN32 )
const char PATH_CHAR_SEPARATOR = '\\';
const char PATH_CHAR_SEPARATOR_WRONG = '/';
#else
// Assumes a Unix-like system (Linux, macOS, etc.) otherwise.
const char PATH_CHAR_SEPARATOR = '/';
const char PATH_CHAR_SEPARATOR_WRONG = '\\';

#endif
// Finds all files with a .h extension in a directory and populates the header_files list.
void find_header_files(const char* path, file_list_t* header_files);

// -----------------------------------------------------------------------------
// Parse
// -----------------------------------------------------------------------------

#define MAX_NAME_LENGTH 256
#define MAX_FIELDS 64
#define MAX_ENUM_VALUES 128
#define MAX_USER_TYPES 128

typedef struct parsed_field_t {
    char type_name[MAX_NAME_LENGTH];
    char name[MAX_NAME_LENGTH];
} parsed_field_t;

typedef struct parsed_enum_value_t {
    char name[MAX_NAME_LENGTH];
} parsed_enum_value_t;

typedef enum parsed_kind_t {
    PARSED_KIND_STRUCT,
    PARSED_KIND_ENUM
} parsed_kind_t;

typedef struct parsed_type_t {
    parsed_kind_t kind;
    char name[MAX_NAME_LENGTH];
    union {
        struct {
            parsed_field_t fields[MAX_FIELDS];
            int num_fields;
        } struct_info;
        struct {
            parsed_enum_value_t values[MAX_ENUM_VALUES];
            int num_values;
        } enum_info;
    };
} parsed_type_t;

// A global structure to hold all parsed data from all files
typedef struct parsed_data_t {
    parsed_type_t types[MAX_USER_TYPES];
    int num_types;
} parsed_data_t;

// Parses a single header file and adds its reflection data to the parsed_data object.
// Returns false on failure.
bool parse_header_file(const char* filepath, parsed_data_t* data);

// -----------------------------------------------------------------------------
// Output
// -----------------------------------------------------------------------------

// Generates the cflex_generated.h and cflex_generated.c files.
// Returns false on failure.
bool generate_output_files(const char* output_path, const parsed_data_t* data, const file_list_t* headers);

// --- Standard Library Wrappers ---

// ctype.h
int char_is_space(int c);
int char_is_alnum(int c);
int char_is_alpha(int c);
int char_is_digit(int c);
int char_to_upper(int c);

// stdio.h
int print_printf(const char* format, ...);
int print_fprintf(FILE* stream, const char* format, ...);
int print_snprintf(char* str, size_t size, const char* format, ...);

// stdlib.h
void* mem_alloc(size_t size);
void* mem_calloc(size_t num, size_t size);
void  mem_free(void* ptr);

// system
char* sys_get_cwd(char* buf, int size);

// string.h
void* mem_copy(void* dest, const void* src, size_t n);
void* mem_set(void* s, int c, size_t n);
int   str_cmp(const char* s1, const char* s2);
int   str_ncmp(const char* s1, const char* s2, size_t n);
size_t str_len(const char* s);
char* str_cpy(char* dest, const char* src);
char* str_ncpy(char* dest, const char* src, size_t n);
char* str_cat(char* dest, const char* src);
char* str_ncat(char* dest, const char* src, size_t n);
char* str_chr(const char* s, int c);
char* str_rchr(const char* s, int c);
char* str_str(const char* haystack, const char* needle);

#endif // CFLEX_INTERNAL_H
