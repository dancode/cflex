// This file contains the core logic for parsing C header files to extract
// reflection data. The parser is a simple, hand-written recursive descent
// parser. It operates on a single file's content loaded into a memory buffer.
// It is not a full C parser; it only looks for specific patterns (`CF_STRUCT()`,
// `CF_ENUM()`) and parses the `typedef struct` and `typedef enum` that follow.

// --- Forward Declarations for Parsers ---
static const char* parse_struct( const char* cursor, parsed_data_t* data );
static const char* parse_enum( const char* cursor, parsed_data_t* data );

// --- Helper Functions ---

// Reads a C identifier (e.g., variable name, type name) from the cursor.
// An identifier is a sequence of alphanumeric characters and underscores.
// The identifier is written into the `buffer`, and the cursor is advanced
// past the identifier.
static const char*
read_identifier( const char* cursor, char* buffer, int32_t buffer_size )
{
    cursor    = str_left_trim( cursor );
    int32_t i = 0;
    while ( *cursor && char_is_identifier( *cursor ) && i < buffer_size - 1 ) { buffer[ i++ ] = *cursor++; }
    buffer[ i ] = '\0';
    return cursor;
}

// --- Main Parsing Logic ---

#define FILE_BUFFER_SIZE ( 1024 * 1024 )    // 1MB buffer for file content

// A static buffer to hold the entire content of a header file.
// Using a static variable avoids allocating a large buffer on the stack.
static char file_buffer[ FILE_BUFFER_SIZE ];

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
                file_print_fmt( stderr, "Error parsing file %s. Aborting.\n", filepath );
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

// --- Type-Specific Parsers ---

// Parses a `typedef struct` block, expecting it to follow a `CF_STRUCT()` annotation.
// It extracts the struct's name and any fields marked with `CF_FIELD()`.

static const char*
parse_struct( const char* cursor, parsed_data_t* data )
{
    cursor = str_left_trim( cursor );

    // Expect "typedef struct"
    if ( str_ncmp( cursor, "typedef struct", 14 ) != 0 )
    {
        return cursor;
    }

    cursor += 14;
    cursor = str_left_trim( cursor );

    char tag_name[ MAX_NAME_LENGTH ];
    cursor = read_identifier( cursor, tag_name, sizeof( tag_name ) );
    cursor = str_left_trim( cursor );

    if ( *cursor != '{' )
        return cursor;
    cursor++;

    const char* body_start = cursor;
    const char* body_end   = str_chr( body_start, '}' );
    if ( !body_end )
        return NULL;

    if ( data->num_types >= MAX_USER_TYPES )
        return NULL;
    parsed_type_t* type          = &data->types[ data->num_types ];
    type->kind                   = PARSED_KIND_STRUCT;
    type->struct_info.num_fields = 0;

    // Parse fields within the struct body
    while ( cursor < body_end )
    {
        const char* field_marker = str_str( cursor, "CF_FIELD()" );
        if ( !field_marker || field_marker > body_end )
            break;

        cursor = field_marker + str_len( "CF_FIELD()" );
        cursor = str_left_trim( cursor );

        if ( type->struct_info.num_fields >= MAX_FIELDS )
            return NULL;
        parsed_field_t* field = &type->struct_info.fields[ type->struct_info.num_fields ];

        cursor                = read_identifier( cursor, field->type_name, MAX_NAME_LENGTH );
        cursor                = read_identifier( cursor, field->name, MAX_NAME_LENGTH );

        char* semicolon       = str_chr( field->name, ';' );
        if ( semicolon )
            *semicolon = '\0';

        type->struct_info.num_fields++;
    }

    cursor          = body_end + 1;    // Move past '}'
    cursor          = str_left_trim( cursor );
    cursor          = read_identifier( cursor, type->name, MAX_NAME_LENGTH );
    char* semicolon = str_chr( type->name, ';' );
    if ( semicolon )
        *semicolon = '\0';

    print_fmt( "Parsed Struct '%s' with %d fields\n", type->name, type->struct_info.num_fields );
    for ( int i = 0; i < type->struct_info.num_fields; i++ )
    {
        print_fmt( "  - Field: %s %s\n", type->struct_info.fields[ i ].type_name,
                   type->struct_info.fields[ i ].name );
    }

    data->num_types++;
    return cursor;
}

// Parses a `typedef enum` block, expecting it to follow a `CF_ENUM()` annotation.
// It extracts the enum's name and all of its values.

static const char*
parse_enum( const char* cursor, parsed_data_t* data )
{
    cursor = str_left_trim( cursor );

    // Expect "typedef enum"
    if ( str_ncmp( cursor, "typedef enum", 12 ) != 0 )
        return cursor;
    cursor += 12;
    cursor = str_left_trim( cursor );

    char tag_name[ MAX_NAME_LENGTH ];
    cursor = read_identifier( cursor, tag_name, sizeof( tag_name ) );
    cursor = str_left_trim( cursor );

    if ( *cursor != '{' )
        return cursor;
    cursor++;

    const char* body_start = cursor;
    const char* body_end   = str_chr( body_start, '}' );
    if ( !body_end )
        return NULL;

    if ( data->num_types >= MAX_USER_TYPES )
        return NULL;
    parsed_type_t* type        = &data->types[ data->num_types ];
    type->kind                 = PARSED_KIND_ENUM;
    type->enum_info.num_values = 0;

    // Parse values
    while ( cursor < body_end )
    {
        const char* next_comma = str_chr( cursor, ',' );
        const char* value_end  = ( next_comma && next_comma < body_end ) ? next_comma : body_end;

        if ( type->enum_info.num_values >= MAX_ENUM_VALUES )
            return NULL;
        parsed_enum_value_t* value = &type->enum_info.values[ type->enum_info.num_values ];

        char    temp[ MAX_NAME_LENGTH ];
        int32_t len = (int32_t)( value_end - cursor );
        if ( len >= MAX_NAME_LENGTH )
            len = MAX_NAME_LENGTH - 1;
        str_copy( temp, cursor, len );
        temp[ len ] = '\0';

        // Strip assignment (e.g., "COLOR_RED = 0")
        char* equals = str_chr( temp, '=' );
        if ( equals )
            *equals = '\0';

        read_identifier( str_left_trim( temp ), value->name, MAX_NAME_LENGTH );

        if ( str_len( value->name ) > 0 )
        {
            type->enum_info.num_values++;
        }

        cursor = value_end;
        if ( *cursor == ',' )
            cursor++;
        if ( cursor >= body_end )
            break;
    }

    cursor          = body_end + 1;
    cursor          = str_left_trim( cursor );
    cursor          = read_identifier( cursor, type->name, MAX_NAME_LENGTH );
    char* semicolon = str_chr( type->name, ';' );
    if ( semicolon )
        *semicolon = '\0';

    print_fmt( "Parsed Enum '%s' with %d values\n", type->name, type->enum_info.num_values );
    for ( int i = 0; i < type->enum_info.num_values; i++ )
    {
        print_fmt( "  - Value: %s\n", type->enum_info.values[ i ].name );
    }

    data->num_types++;
    return cursor;
}
