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

// -----------------------------------------------------------------------------
// Scan
// -----------------------------------------------------------------------------
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

#endif // CFLEX_INTERNAL_H
