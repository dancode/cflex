/*==============================================================================================

    Parse Struct

==============================================================================================*/

static const char*
parse_field( const char* cursor, parsed_type_t* type )
{
    if ( type->struct_info.num_fields >= MAX_FIELDS )
    {
        print_fmt( "Error: too many fields in struct\n" );
        return NULL;
    }

    parsed_field_t* field = &type->struct_info.fields[ type->struct_info.num_fields ];

    cursor                = str_left_trim( cursor );
    cursor                = read_identifier( cursor, field->type_name, MAX_NAME_LENGTH );
    cursor                = read_identifier( cursor, field->name, MAX_NAME_LENGTH );

    // Strip (optional) trailing ';' from string name.
    {
        char* semi = strchr( field->name, ';' );
        if ( semi )
            *semi = '\0';
    }

    type->struct_info.num_fields++;
    return cursor;
}

/*============================================================================================*/

static const char*
parse_struct( const char* cursor, parsed_data_t* data )
{
    bool is_typedef           = false;

    cursor                    = str_left_trim( cursor );
    const char* after_typedef = optional_keyword( cursor, "typedef" );
    if ( after_typedef )
    {
        is_typedef = true;
        cursor     = str_left_trim( after_typedef );
    }

    cursor = str_left_trim( cursor );
    cursor = expect_keyword( cursor, "struct" );
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
        print_fmt( "Parse error:: struct missing closing brace\n" );
        return NULL;
    }

    if ( data->num_types >= MAX_USER_TYPES )
    {
        print_fmt( "Parse error:: too many user types\n" );
        return NULL;
    }

    // Add struct type to type list
    parsed_type_t* type          = &data->types[ data->num_types ];
    type->kind                   = PARSED_KIND_STRUCT;
    type->struct_info.num_fields = 0;

    // Parse CF_FIELD() inside body
    while ( cursor < body_end )
    {
        const char* marker = memchr( cursor, 'C', body_end - cursor );
        if ( !marker )
            break;

        if ( str_ncmp( marker, "CF_FIELD()", 10 ) != 0 )
        {
            cursor = marker + 1;
            continue;
        }

        cursor = marker + 10;
        cursor = parse_field( cursor, type );
        if ( !cursor )
            return NULL;
    }

    cursor = expect_char( body_end, '}' );
    if ( !cursor )
        return NULL;

    if ( is_typedef )
    {
        // typedef struct { … } name;
        cursor = read_identifier( cursor, type->name, MAX_NAME_LENGTH );
        if ( str_len( type->name ) == 0 )
        {
            print_fmt( "Parse error: typedef struct missing closing identifier\n" );
            return NULL;
        }

        cursor = str_left_trim( cursor );
        cursor = expect_char( cursor, ';' );
        if ( !cursor )
        {
            print_fmt( "Parse error: typedef struct missing semicolon\n" );
            return NULL;
        }
    }
    else
    {
        // enum name { … };
        if ( str_len( tag_name ) == 0 )
        {
            print_fmt( "Parse error: non-typedef struct must have a tag name\n" );
            return NULL;
        }

        str_copy( type->name, tag_name, MAX_NAME_LENGTH );

        cursor = str_left_trim( cursor );
        cursor = expect_char( cursor, ';' );
        if ( !cursor )
        {
            print_fmt( "Parse error: struct expected semicolon\n" );
            return NULL;
        }
    }

    if ( CFLEX_DEBUG_PRINT )
    {
        print_fmt( "Parsed Struct '%s' with %d fields\n", type->name, type->struct_info.num_fields );
        for ( int i = 0; i < type->struct_info.num_fields; i++ )
        {
            print_fmt( "  - Field: %s %s\n", type->struct_info.fields[ i ].type_name,
                       type->struct_info.fields[ i ].name );
        }
    }

    data->num_types++;
    return cursor;
}

/*============================================================================================*/