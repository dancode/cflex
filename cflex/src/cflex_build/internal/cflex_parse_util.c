/*==============================================================================================

    Parser Utility Functions

==============================================================================================*/

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

/*============================================================================================*/

// Parse the next token keyword and check if it is was we expect

static const char*
expect_keyword( const char* cursor, const char* keyword )
{
    cursor      = str_left_trim( cursor );
    int32_t len = str_len( keyword );
    if ( str_ncmp( cursor, keyword, len ) != 0 )
    {
        print_fmt( "Parse error: expected keyword '%s'\n", keyword );
        return NULL;
    }
    return cursor + len;
}

/*============================================================================================*/

static const char*
expect_char( const char* cursor, char c )
{
    cursor = str_left_trim( cursor );
    if ( *cursor != c )
    {
        print_fmt( "Parse error: expected '%c'\n", c );
        return NULL;
    }
    return cursor + 1;
}

/*============================================================================================*/