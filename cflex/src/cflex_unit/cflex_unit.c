#include "cflex_unit_types.h"
#include "cflex.h"
#include "cflex_unit_generated.h"

#include <stdio.h>
#include <string.h>

// --- Minimal Test Framework ---
#define TEST_ASSERT( condition )                                                      \
    do {                                                                              \
        if ( !( condition ) )                                                         \
        {                                                                             \
            printf( "ASSERT FAILED: %s at %s:%d\n", #condition, __FILE__, __LINE__ ); \
            return 1;                                                                 \
        }                                                                             \
    }                                                                                 \
    while ( 0 )

#define RUN_TEST( test_func )                          \
    do {                                               \
        printf( "Running test: %s...\n", #test_func ); \
        if ( test_func() == 0 )                        \
        {                                              \
            printf( "  - PASSED\n" );                  \
        }                                              \
        else                                           \
        {                                              \
            printf( "  - FAILED\n" );                  \
            return 1;                                  \
        }                                              \
    }                                                  \
    while ( 0 )

// --- Test Cases ---

int
test_find_type_by_name()
{
    const cf_type_t* test_struct_type = cf_find_type_by_name( "test_struct_t" );
    TEST_ASSERT( test_struct_type != NULL );
    TEST_ASSERT( strcmp( test_struct_type->name, "test_struct_t" ) == 0 );
    TEST_ASSERT( test_struct_type->kind == CF_KIND_STRUCT );

    const cf_type_t* non_existent = cf_find_type_by_name( "non_existent_type" );
    TEST_ASSERT( non_existent == NULL );

    return 0;
}

int
test_table_api()
{
    int32_t num_tables = cf_get_num_tables();
    TEST_ASSERT( num_tables > 0 );

    const cf_type_t** types;
    int32_t           count;
    cf_get_table( 0, &types, &count );
    TEST_ASSERT( count > 0 );
    TEST_ASSERT( types != NULL );

    return 0;
}

int
test_find_field()
{
    const cf_type_t* test_struct_type = cf_find_type_by_name( "test_struct_t" );
    TEST_ASSERT( test_struct_type != NULL );

    const cf_field_t* field_v = cf_find_field( test_struct_type, "v" );
    TEST_ASSERT( field_v != NULL );
    TEST_ASSERT( strcmp( field_v->name, "v" ) == 0 );
    TEST_ASSERT( strcmp( field_v->type->name, "test_vec2_t" ) == 0 );

    const cf_field_t* non_existent = cf_find_field( test_struct_type, "non_existent_field" );
    TEST_ASSERT( non_existent == NULL );
    return 0;
}

int
test_find_enum_value()
{
    const cf_type_t* enum_type = cf_find_type_by_name( "test_enum_t" );
    TEST_ASSERT( enum_type != NULL );
    TEST_ASSERT( enum_type->kind == CF_KIND_ENUM );

    const cf_enum_value_t* val_b = cf_find_enum_value_by_name( enum_type, "TEST_ENUM_B" );
    TEST_ASSERT( val_b != NULL );
    TEST_ASSERT( strcmp( val_b->name, "TEST_ENUM_B" ) == 0 );
    TEST_ASSERT( val_b->value == 1 );

    const cf_enum_value_t* non_existent_name = cf_find_enum_value_by_name( enum_type, "non_existent" );
    TEST_ASSERT( non_existent_name == NULL );

    return 0;
}

int
main()
{
    cf_initialize();
    cflex_unit_register_types();

    printf( "--- Running C-Flex Unit Tests ---\n" );
    RUN_TEST( test_find_type_by_name );
    RUN_TEST( test_table_api );
    RUN_TEST( test_find_field );
    RUN_TEST( test_find_enum_value );
    printf( "---------------------------------\n" );
    printf( "All tests passed!\n" );

    cf_shutdown();
    return 0;
}
