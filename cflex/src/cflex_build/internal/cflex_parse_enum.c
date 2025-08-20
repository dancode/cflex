/*==============================================================================================

    Parse Enum

==============================================================================================*/

static const char*
parse_enum_value( const char* cursor, parsed_type_t* type, int32_t* last_value )
{
    if ( type->enum_info.num_values >= MAX_ENUM_VALUES )
    {
        print_fmt( "Error: too many enum values in enum\n" );
        return NULL;
    }

    cursor = str_left_trim( cursor );
    if ( !*cursor || *cursor == '}' )
    {
        return cursor;
    }

    const char*      name_start = cursor;
    const char*      name_end   = cursor;
    const char*      equals     = NULL;
    parsed_enum_value_t* value      = &type->enum_info.values[ type->enum_info.num_values ];

    // Find the end of the identifier and the optional equals sign
    while ( *cursor && *cursor != ',' && *cursor != '}' )
    {
        if ( *cursor == '=' )
        {
            equals = cursor;
        }
        cursor++;
    }
    name_end = equals ? equals : cursor;

    // Trim trailing whitespace from the name
    while ( name_end > name_start && char_is_space( *( name_end - 1 ) ) )
    {
        name_end--;
    }

    str_copy_sub( value->name, name_start, (int32_t)( name_end - name_start ), MAX_NAME_LENGTH );

    if ( str_len( value->name ) == 0 )
    {
        return cursor; // Skip empty entries from multiple commas
    }

    if ( equals )
    {
        const char* value_str = str_left_trim( equals + 1 );
        value->value          = atoi( value_str );
    }
    else
    {
        value->value = *last_value;
    }

    *last_value = value->value + 1;
    type->enum_info.num_values++;

    return cursor;
}

/*============================================================================================*/

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

    parsed_type_t* type        = &data->types[ data->num_types ];
    type->kind                 = PARSED_KIND_ENUM;
    type->enum_info.num_values = 0;
    int32_t last_value         = 0;

    while ( cursor < body_end )
    {
        const char* next_comma = str_chr( cursor, ',' );
        const char* value_end  = ( next_comma && next_comma < body_end ) ? next_comma : body_end;

        cursor = parse_enum_value( cursor, type, &last_value );
        if ( !cursor )
            return NULL;

        if ( *cursor == ',' )
            cursor++;
        else
            cursor = value_end;
    }

    cursor = expect_char( body_end, '}' );
    if ( !cursor )
        return NULL;

    cursor = str_left_trim( cursor );
    cursor = read_identifier( cursor, type->name, MAX_NAME_LENGTH );

    char* semi = strchr( type->name, ';' );
    if ( semi )
        *semi = '\0';

    data->num_types++;
    return cursor;
}

/*============================================================================================*/
