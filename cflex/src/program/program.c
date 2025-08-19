#include "program.h"

#include <stdio.h>

#include "cflex.h"
#include "cflex_implementation.h"

void
print_type_details( const cf_type_t* type )
{
    if ( !type )
    {
        printf( "Type not found.\n" );
        return;
    }

    printf( "Details for '%s':\n", type->name );
    printf( "  - Kind: %d\n", type->kind );
    printf( "  - Size: %d bytes\n", type->size );
    printf( "  - Alignment: %d bytes\n", type->align );

    if ( type->kind == CF_KIND_STRUCT )
    {
        printf( "  - Fields (%d):\n", type->struct_count );
        for ( int32_t i = 0; i < type->struct_count; ++i )
        {
            const cf_field_t* field = &type->struct_array[ i ];
            printf( "    - %s (type: %s, offset: %d)\n", field->name, field->type->name, field->offset );
        }
    }
    else if ( type->kind == CF_KIND_ENUM )
    {
        printf( "  - Values (%d):\n", type->enum_count );
        for ( int32_t i = 0; i < type->enum_count; ++i )
        {
            const cf_enum_value_t* val = &type->enum_array[ i ];
            printf( "    - %s = %d\n", val->name, val->value );
        }
    }
}

void
print_all_types()
{
    const cf_type_t** types;
    int32_t           count;
    cf_get_all_types( &types, &count );

    printf( "--- All Registered Types (%d) ---\n", count );
    for ( int32_t i = 0; i < count; ++i )
    {
        print_type_details( types[ i ] );
        printf( "\n" );
    }
    printf( "---------------------------------\n" );
}

int
main( int argc, char** argv )
{
    (void)argc;
    (void)argv;

    printf( "C-Flex Reflection System Demo\n" );
    printf( "-----------------------------\n" );
    printf( "Demonstrating library API usage:\n\n" );

    const cf_type_t* player_type = cf_find_type_by_name( "player_t" );
    print_type_details( player_type );
    printf( "\n" );

    const cf_type_t* color_type = cf_find_type_by_name( "color_t" );
    print_type_details( color_type );
    printf( "\n" );

    const cf_type_t* float_type = cf_find_type_by_name( "float" );
    print_type_details( float_type );
    printf( "\n" );

    printf( "Searching for 'non_existent_type'...\n" );
    const cf_type_t* non_existent_type = cf_find_type_by_name( "non_existent_type" );
    print_type_details( non_existent_type );
    printf( "\n" );

    printf( "Searching for type by ID: CF_TYPE_ID_PLAYER_T...\n" );
    const cf_type_t* player_by_id = cf_find_type_by_id( CF_TYPE_ID_PLAYER_T );
    print_type_details( player_by_id );
    printf( "\n" );

    // --- New ---
    // Print all types in the system
    print_all_types();

    return 0;
}
