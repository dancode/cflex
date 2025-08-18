#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define FILE_BUFFER_SIZE (1024 * 1024) // 1MB buffer for file content
static char file_buffer[FILE_BUFFER_SIZE];

// --- Forward Declarations for Parsers ---
static const char* parse_struct(const char* cursor, parsed_data_t* data);
static const char* parse_enum(const char* cursor, parsed_data_t* data);

// --- Helper Functions ---

// Reads a C identifier from the cursor into the buffer.
static const char* read_identifier(const char* cursor, char* buffer, size_t buffer_size) {
    cursor = ltrim(cursor);
    size_t i = 0;
    while (*cursor && is_identifier_char(*cursor) && i < buffer_size - 1) {
        buffer[i++] = *cursor++;
    }
    buffer[i] = '\0';
    return cursor;
}

// --- Main Parsing Logic ---

bool parse_header_file(const char* filepath, parsed_data_t* data) {
    FILE* fp = fopen(filepath, "r");
    if (!fp) {
        fprintf(stderr, "Error: Could not open file %s\n", filepath);
        return false;
    }

    size_t bytes_read = fread(file_buffer, 1, FILE_BUFFER_SIZE - 1, fp);
    file_buffer[bytes_read] = '\0';
    fclose(fp);

    const char* cursor = file_buffer;

    while (*cursor) {
        const char* next_struct = strstr(cursor, "CF_STRUCT()");
        const char* next_enum = strstr(cursor, "CF_ENUM()");

        const char* next_annotation = NULL;
        bool is_struct = false;

        if (next_struct && (!next_enum || next_struct < next_enum)) {
            next_annotation = next_struct;
            is_struct = true;
        } else if (next_enum) {
            next_annotation = next_enum;
            is_struct = false;
        } else {
            break; // No more annotations found
        }

        if (is_struct) {
            cursor = parse_struct(next_annotation + strlen("CF_STRUCT()"), data);
        } else {
            cursor = parse_enum(next_annotation + strlen("CF_ENUM()"), data);
        }

        if (!cursor) {
            fprintf(stderr, "Error parsing file %s. Aborting.\n", filepath);
            return false;
        }
    }

    return true;
}

// --- Type-Specific Parsers ---

static const char* parse_struct(const char* cursor, parsed_data_t* data) {
    cursor = ltrim(cursor);

    if (strncmp(cursor, "typedef struct", 14) != 0) return cursor;
    cursor += 14;
    cursor = ltrim(cursor);

    char tag_name[MAX_NAME_LENGTH];
    cursor = read_identifier(cursor, tag_name, sizeof(tag_name));
    cursor = ltrim(cursor);

    if (*cursor != '{') return cursor;
    cursor++;

    const char* body_start = cursor;
    const char* body_end = strchr(body_start, '}');
    if (!body_end) return NULL;

    if (data->num_types >= MAX_USER_TYPES) return NULL;
    parsed_type_t* type = &data->types[data->num_types];
    type->kind = PARSED_KIND_STRUCT;
    type->struct_info.num_fields = 0;

    // Parse fields within the struct body
    while (cursor < body_end) {
        const char* field_marker = strstr(cursor, "CF_FIELD()");
        if (!field_marker || field_marker > body_end) break;

        cursor = field_marker + strlen("CF_FIELD()");
        cursor = ltrim(cursor);

        if (type->struct_info.num_fields >= MAX_FIELDS) return NULL;
        parsed_field_t* field = &type->struct_info.fields[type->struct_info.num_fields];

        cursor = read_identifier(cursor, field->type_name, MAX_NAME_LENGTH);
        cursor = read_identifier(cursor, field->name, MAX_NAME_LENGTH);

        char* semicolon = strchr(field->name, ';');
        if (semicolon) *semicolon = '\0';

        type->struct_info.num_fields++;
    }

    cursor = body_end + 1; // Move past '}'
    cursor = ltrim(cursor);
    cursor = read_identifier(cursor, type->name, MAX_NAME_LENGTH);
    char* semicolon = strchr(type->name, ';');
    if (semicolon) *semicolon = '\0';

    printf("Parsed Struct '%s' with %d fields\n", type->name, type->struct_info.num_fields);
    for (int i = 0; i < type->struct_info.num_fields; i++) {
        printf("  - Field: %s %s\n", type->struct_info.fields[i].type_name, type->struct_info.fields[i].name);
    }

    data->num_types++;
    return cursor;
}

static const char* parse_enum(const char* cursor, parsed_data_t* data) {
    cursor = ltrim(cursor);

    if (strncmp(cursor, "typedef enum", 12) != 0) return cursor;
    cursor += 12;
    cursor = ltrim(cursor);

    char tag_name[MAX_NAME_LENGTH];
    cursor = read_identifier(cursor, tag_name, sizeof(tag_name));
    cursor = ltrim(cursor);

    if (*cursor != '{') return cursor;
    cursor++;

    const char* body_start = cursor;
    const char* body_end = strchr(body_start, '}');
    if (!body_end) return NULL;

    if (data->num_types >= MAX_USER_TYPES) return NULL;
    parsed_type_t* type = &data->types[data->num_types];
    type->kind = PARSED_KIND_ENUM;
    type->enum_info.num_values = 0;

    // Parse values
    while(cursor < body_end) {
        const char* next_comma = strchr(cursor, ',');
        const char* value_end = (next_comma && next_comma < body_end) ? next_comma : body_end;

        if (type->enum_info.num_values >= MAX_ENUM_VALUES) return NULL;
        parsed_enum_value_t* value = &type->enum_info.values[type->enum_info.num_values];

        char temp[MAX_NAME_LENGTH];
        size_t len = value_end - cursor;
        if (len >= MAX_NAME_LENGTH) len = MAX_NAME_LENGTH - 1;
        strncpy(temp, cursor, len);
        temp[len] = '\0';

        // Strip assignment (e.g., "COLOR_RED = 0")
        char* equals = strchr(temp, '=');
        if (equals) *equals = '\0';

        read_identifier(ltrim(temp), value->name, MAX_NAME_LENGTH);

        if (strlen(value->name) > 0) {
            type->enum_info.num_values++;
        }

        cursor = value_end;
        if (*cursor == ',') cursor++;
        if (cursor >= body_end) break;
    }

    cursor = body_end + 1;
    cursor = ltrim(cursor);
    cursor = read_identifier(cursor, type->name, MAX_NAME_LENGTH);
    char* semicolon = strchr(type->name, ';');
    if (semicolon) *semicolon = '\0';

    printf("Parsed Enum '%s' with %d values\n", type->name, type->enum_info.num_values);
     for (int i = 0; i < type->enum_info.num_values; i++) {
        printf("  - Value: %s\n", type->enum_info.values[i].name);
    }

    data->num_types++;
    return cursor;
}
