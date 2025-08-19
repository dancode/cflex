/*==============================================================================================

    Standard Library Wrappers
    The cflex_build tool uses wrappers around standard C library functions.
    This is done for several reasons:

    1. Consistency: Provides a consistent API within the cflex_build codebase.
    2. Safety: Allows for the future addition of enhanced error checking or
       safer alternatives (e.g., bounds-checked string operations).
    3. Portability: While not a primary goal for this tool, it abstracts away
       the standard library, which can make porting to different platforms or
       compilers easier in the future.
    4. Extensibility: Custom logic, such as memory tracking or logging, can be
       easily added to these wrappers.

==============================================================================================*/

#include "cflex_internal.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/*==============================================================================================

    ctype.h (characters)

==============================================================================================*/

int
char_is_space( int c )
{
    return isspace( c );
}

int
char_is_alnum( int c )
{
    return isalnum( c );
}

int
char_is_alpha( int c )
{
    return isalpha( c );
}

int
char_is_digit( int c )
{
    return isdigit( c );
}

int
char_to_upper( int c )
{
    return toupper( c );
}

// --- ctype.h (extended) ---

bool
char_is_identifier( char c )
{
    return char_is_alnum( (unsigned char)c ) || c == '_';
}

/*==============================================================================================

    stdio.h (printing)

==============================================================================================*/

int
file_print_fmt( FILE* stream, const char* format, ... )
{
    va_list args;
    va_start( args, format );
    int result = vfprintf( stream, format, args );
    va_end( args );
    return result;
}

int
print_fmt( const char* format, ... )
{
    va_list args;
    va_start( args, format );
    int result = vprintf( format, args );
    va_end( args );
    return result;
}

int
str_print_fmt( char* str, int32_t size, const char* format, ... )
{
    va_list args;
    va_start( args, format );
    int result = vsnprintf( str, size, format, args );
    va_end( args );
    return result;
}

/*==============================================================================================

    stdlib.h (memory)

==============================================================================================*/

void*
mem_alloc( int32_t size )
{
    return malloc( size );
}

void*
mem_calloc( int32_t num, int32_t size )
{
    return calloc( num, size );
}

void
mem_free( void* ptr )
{
    free( ptr );
}

void*
mem_copy( void* dest, const void* src, int32_t n )
{
    return memcpy( dest, src, n );
}

void*
mem_set( void* s, int c, int32_t n )
{
    return memset( s, c, n );
}

/*==============================================================================================

    string.h (strings)

==============================================================================================*/

// Compare two null-terminated strings lexicographically
int32_t
str_cmp( const char* s1, const char* s2 )
{
    return strcmp( s1, s2 );
}

// Compare up to n characters of two strings lexicographically
int32_t
str_ncmp( const char* s1, const char* s2, int32_t n )
{
    return strncmp( s1, s2, n );
}

// Return the length of a string as a 32-bit integer
int32_t
str_len( const char* s )
{
    return (int32_t)strlen( s );
}

// Copy a null-terminated string from src to dest
char*
str_cpy( char* dest, const char* src )
{
    return strcpy( dest, src );
}

// Copy up to n characters from src to dest
char*
str_ncpy( char* dest, const char* src, int32_t n )
{
    return strncpy( dest, src, n );
}

// Append src to the end of dest
char*
str_cat( char* dest, const char* src )
{
    return strcat( dest, src );
}

// Append up to n characters of src to the end of dest
char*
str_ncat( char* dest, const char* src, int32_t n )
{
    return strncat( dest, src, n );
}

// Locate the first occurrence of character c in string s
char*
str_chr( const char* s, int c )
{
    return strchr( s, c );
}

// Locate the last occurrence of character c in string s
char*
str_rchr( const char* s, int c )
{
    return strrchr( s, c );
}

// Locate the first occurrence of substring needle in haystack
char*
str_str( const char* haystack, const char* needle )
{
    return strstr( haystack, needle );
}
// --- string.h (extended) ---

// Safely copies a null-terminated string from src to dst.
// Ensures the destination buffer is always null-terminated.
void
str_copy( char* dst, const char* src, int32_t dst_size )
{
    if ( dst_size == 0 )
        return;
    str_ncpy( dst, src, dst_size - 1 );
    dst[ dst_size - 1 ] = '\0';
}

// Safely copies at most `len` characters from src to dst.
// Ensures the destination buffer is always null-terminated.
void
str_copy_sub( char* dst, const char* src, int32_t len, int32_t dst_size )
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

// Returns a pointer to the first non-whitespace character in a string.
const char*
str_left_trim( const char* str )
{
    while ( *str && char_is_space( (unsigned char)*str ) ) { str++; }
    return str;
}

// Simple helper to check if a string ends with a suffix.
bool
str_ends_with( const char* str, const char* suffix )
{
    if ( !str || !suffix )
    {
        return false;
    }
    int32_t s_len   = str_len( str );
    int32_t suf_len = str_len( suffix );
    if ( suf_len > s_len )
    {
        return false;
    }
    return str_ncmp( str + s_len - suf_len, suffix, suf_len ) == 0;
}

/*============================================================================================*/