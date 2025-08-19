#ifndef CFLEX_IMPLEMENTATION_H
#define CFLEX_IMPLEMENTATION_H

#include "cflex.h"
#include <string.h>

// The generated source file is included here to provide the reflection data.
#include "cflex_generated.c"

// Include internal implementation files (unity build style).
#include "internal/cflex_parser.h"

const cf_type_t*
cf_find_type_by_name( const char* name )
{
    if ( !name )
    {
        return NULL;
    }

    for ( int32_t i = 0; i < cf_type_count; ++i )
    {
        const cf_type_t* type = cf_type_array[ i ];
        if ( type && type->name && strcmp( name, type->name ) == 0 )
        {
            return type;
        }
    }

    return NULL;
}

const cf_type_t*
cf_find_type_by_id( cf_type_id_t id )
{
    if ( id >= 0 && id < CF_TYPE_ID_COUNT )
    {
        return cf_type_array[ id ];
    }
    return NULL;
}

void
cf_get_all_types( const cf_type_t*** out_types, int32_t* out_count )
{
    if ( out_types )
    {
        *out_types = cf_type_array;
    }
    if ( out_count )
    {
        *out_count = cf_type_count;
    }
}

const cf_field_t*
cf_find_field( const cf_type_t* type, const char* name )
{
    if ( type && type->kind == CF_KIND_STRUCT && name )
    {
        for ( int32_t i = 0; i < type->struct_count; ++i )
        {
            const cf_field_t* field = &type->struct_array[ i ];
            if ( strcmp( field->name, name ) == 0 )
            {
                return field;
            }
        }
    }
    return NULL;
}

const cf_enum_value_t*
cf_find_enum_value_by_name( const cf_type_t* type, const char* name )
{
    if ( type && type->kind == CF_KIND_ENUM && name )
    {
        for ( int32_t i = 0; i < type->enum_count; ++i )
        {
            const cf_enum_value_t* enum_value = &type->enum_array[ i ];
            if ( strcmp( enum_value->name, name ) == 0 )
            {
                return enum_value;
            }
        }
    }
    return NULL;
}

const cf_enum_value_t*
cf_find_enum_value_by_value( const cf_type_t* type, int32_t value )
{
    if ( type && type->kind == CF_KIND_ENUM )
    {
        for ( int32_t i = 0; i < type->enum_count; ++i )
        {
            const cf_enum_value_t* enum_value = &type->enum_array[ i ];
            if ( enum_value->value == value )
            {
                return enum_value;
            }
        }
    }
    return NULL;
}

#endif    // CFLEX_IMPLEMENTATION_H
