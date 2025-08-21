#ifndef CFLEX_IMPLEMENTATION_H
#define CFLEX_IMPLEMENTATION_H

#include "internal/cflex_internal.h"
#include <string.h>
#include <stdlib.h>

// --- Internal State ---

#define MAX_TYPE_TABLES 16

typedef struct cf_type_table_t
{
    const cf_type_t** types;
    int32_t           count;
} cf_type_table_t;

typedef struct cf_registry_t
{
    cf_type_table_t tables[ MAX_TYPE_TABLES ];
    int32_t         num_tables;
} cf_registry_t;

static cf_registry_t* g_registry = NULL;

// --- API Implementation ---

void
cf_initialize( void )
{
    if ( !g_registry )
    {
        g_registry = (cf_registry_t*)malloc( sizeof( cf_registry_t ) );
        if ( g_registry != NULL )
        {
            memset( g_registry, 0, sizeof( cf_registry_t ) );
        }
    }
}

void
cf_shutdown( void )
{
    if ( g_registry )
    {
        free( g_registry );
        g_registry = NULL;
    }
}

void
cf_register_type_table( const cf_type_t* types[], int32_t count )
{
    if ( g_registry && g_registry->num_tables < MAX_TYPE_TABLES )
    {
        cf_type_table_t* table = &g_registry->tables[ g_registry->num_tables++ ];
        table->types           = types;
        table->count           = count;
    }
}

const cf_type_t*
cf_find_type_by_name( const char* name )
{
    if ( !g_registry || !name )
    {
        return NULL;
    }

    for ( int32_t i = 0; i < g_registry->num_tables; ++i )
    {
        const cf_type_table_t* table = &g_registry->tables[ i ];
        for ( int32_t j = 0; j < table->count; ++j )
        {
            const cf_type_t* type = table->types[ j ];
            if ( type && type->name && strcmp( name, type->name ) == 0 )
            {
                return type;
            }
        }
    }

    return NULL;
}

int32_t
cf_get_num_tables( void )
{
    if ( !g_registry )
    {
        return 0;
    }
    return g_registry->num_tables;
}

void
cf_get_table( int32_t table_index, const cf_type_t*** out_types, int32_t* out_count )
{
    if ( out_types )
        *out_types = NULL;
    if ( out_count )
        *out_count = 0;

    if ( g_registry && table_index >= 0 && table_index < g_registry->num_tables )
    {
        if ( out_types )
            *out_types = g_registry->tables[ table_index ].types;
        if ( out_count )
            *out_count = g_registry->tables[ table_index ].count;
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
