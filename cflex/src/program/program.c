#include "program.h"

#include <stdio.h>

#include "cflex.h"
#include "cflex_default_generated.h"
#include "program_generated.h"

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
    printf( "--- All Registered Types ---\n" );
    int32_t num_tables = cf_get_num_tables();
    for ( int32_t i = 0; i < num_tables; ++i )
    {
        const cf_type_t** types;
        int32_t           count;
        cf_get_table( i, &types, &count );

        printf( "--- Table %d (%d types) ---\n", i, count );
        for ( int32_t j = 0; j < count; ++j )
        {
            print_type_details( types[ j ] );
            printf( "\n" );
        }
    }
    printf( "---------------------------------\n" );
}

int
main( int argc, char** argv )
{
    (void)argc;
    (void)argv;

    cf_initialize();
    cflex_default_register_types();
    program_register_types();

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

    // --- New ---
    // Print all types in the system
    print_all_types();

    cf_shutdown();
    return 0;
}
