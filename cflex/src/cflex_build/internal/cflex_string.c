const char*
ltrim( const char* str )
{
    while ( *str && char_is_space( (unsigned char)*str ) ) { str++; }
    return str;
}

bool
is_identifier_char( char c )
{
    return char_is_alnum( (unsigned char)c ) || c == '_';
}

void
str_copy( char* dst, const char* src, int32_t dst_size )
{
    if ( dst_size == 0 )
        return;
    str_ncpy( dst, src, dst_size - 1 );
    dst[ dst_size - 1 ] = '\0';
}

void
substr_copy( char* dst, const char* src, int32_t len, int32_t dst_size )
{
    if ( dst_size == 0 )
        return;

    int32_t copy_len = len;
    if ( copy_len >= dst_size )
    {
        copy_len = dst_size - 1;
    }

    mem_copy( dst, src, copy_len );
    dst[ copy_len ] = '\0';
}
