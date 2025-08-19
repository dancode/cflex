/*==============================================================================================

    Parse Enum

==============================================================================================*/

// Helper: parse one enum value between cursor and value_end.
// Advances cursor to just after the value.

static const char*
parse_enum_value( const char* cursor, const char* value_end, parsed_type_t* type )
{
    if ( type->enum_info.num_values >= MAX_ENUM_VALUES )
    {
        print_fmt( "Error: too enum values in enum\n" );
        return NULL;
    }

    parsed_enum_value_t* value = &type->enum_info.values[ type->enum_info.num_values ];

    cursor                     = str_left_trim( cursor );

    // Copy raw token into temp.
    char temp[ MAX_NAME_LENGTH ];
    {
        int32_t len = (int)( value_end - cursor );
        if ( len >= MAX_NAME_LENGTH )
        {
            len = MAX_NAME_LENGTH - 1;
        }
        str_copy( temp, cursor, len );
        temp[ len ] = '\0';
    }

    // Strip assignment (e.g., "VALUE = 1")
    {
        char* equals = str_chr( temp, '=' );
        if ( equals )
            *equals = '\0';
    }

    // Extract identifier
    read_identifier( str_left_trim( temp ), value->name, MAX_NAME_LENGTH );

    if ( str_len( value->name ) > 0 )
    {
        type->enum_info.num_values++;
    }

    return value_end;
}

/*============================================================================================*/

// Parses a `typedef enum` block, expecting it to follow a `CF_ENUM()` annotation.
// It extracts the enum's name and all of its values.

static const char*
parse_enum( const char* cursor, parsed_data_t* data )
{
    cursor = str_left_trim( cursor );
    cursor = expect_keyword( cursor, "typedef" );
    if ( !cursor )
        return NULL;

    cursor = str_left_trim( cursor );
    cursor = expect_keyword( cursor, "enum" );
    if ( !cursor )
        return NULL;

    // Optional tag name (may be typedef at end of struct definition)

    char tag_name[ MAX_NAME_LENGTH ];
    cursor = str_left_trim( cursor );
    cursor = read_identifier( cursor, tag_name, sizeof( tag_name ) );
    cursor = str_left_trim( cursor );

    cursor = expect_char( cursor, '{' );
    if ( !cursor )
        return NULL;

    const char* body_start = cursor;
    const char* body_end   = str_chr( body_start, '}' );
    if ( !body_end )
    {
        print_fmt( "Error: enum missing closing brace\n" );
        return NULL;
    }

    if ( data->num_types >= MAX_USER_TYPES )
    {
        print_fmt( "Error: too many user types\n" );
        return NULL;
    }

    // Add enum type to type list
    parsed_type_t* type        = &data->types[ data->num_types ];
    type->kind                 = PARSED_KIND_ENUM;
    type->enum_info.num_values = 0;

    // Parse CF_ENUM() inside body
    while ( cursor < body_end )
    {
        const char* next_comma = str_chr( cursor, ',' );
        const char* value_end  = ( next_comma && next_comma < body_end ) ? next_comma : body_end;

        // custom parse for value
        cursor = parse_enum_value( cursor, value_end, type );
        if ( !cursor )
            return NULL;

        // skip trailing comma (found another enum value)
        if ( *cursor == ',' )
            cursor++;

        cursor = str_left_trim( cursor );
    }

    // Move past '}'
    cursor = expect_char( body_end, '}' );
    if ( !cursor )
        return NULL;

    // Read typedef enum name.
    cursor = str_left_trim( cursor );
    cursor = read_identifier( cursor, type->name, MAX_NAME_LENGTH );

    // Strip (optional) trailing ';' from string name.
    {
        char* semi = strchr( type->name, ';' );
        if ( semi )
            *semi = '\0';
    }

    if ( CFLEX_BUILD_DEBUG )
    {
        // Debug print
        print_fmt( "Parsed Enum '%s' with %d values\n", type->name, type->enum_info.num_values );
        for ( int i = 0; i < type->enum_info.num_values; i++ )
        {
            print_fmt( "  - Value: %s\n", type->enum_info.values[ i ].name );
        }
    }

    data->num_types++;
    return cursor;
}

/*============================================================================================*/
