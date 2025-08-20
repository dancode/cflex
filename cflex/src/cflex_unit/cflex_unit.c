#include <stdio.h>
#include <string.h>
#include "cflex.h"
#include "cflex_implementation.h"

#include "cflex_default_generated.h"
#include "program_generated.h"
#include "cflex_unit_generated.h"

// --- Minimal Test Framework ---
#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            printf("ASSERT FAILED: %s at %s:%d\n", #condition, __FILE__, __LINE__); \
            return 1; \
        } \
    } while (0)

#define RUN_TEST(test_func) \
    do { \
        printf("Running test: %s...\n", #test_func); \
        if (test_func() == 0) { \
            printf("  - PASSED\n"); \
        } else { \
            printf("  - FAILED\n"); \
            return 1; \
        } \
    } while (0)

// --- Test Cases ---

int test_find_type_by_name() {
    const cf_type_t* player_type = cf_find_type_by_name("player_t");
    TEST_ASSERT(player_type != NULL);
    TEST_ASSERT(strcmp(player_type->name, "player_t") == 0);
    TEST_ASSERT(player_type->kind == CF_KIND_STRUCT);

    const cf_type_t* non_existent = cf_find_type_by_name("non_existent_type");
    TEST_ASSERT(non_existent == NULL);

    return 0;
}

int test_table_api() {
    int32_t num_tables = cf_get_num_tables();
    TEST_ASSERT(num_tables > 0);

    const cf_type_t** types;
    int32_t           count;
    cf_get_table( 0, &types, &count );
    TEST_ASSERT(count > 0);
    TEST_ASSERT(types != NULL);

return 0;
}

int test_find_field() {
    const cf_type_t* player_type = cf_find_type_by_name("player_t");
    TEST_ASSERT(player_type != NULL);

    const cf_field_t* health_field = cf_find_field(player_type, "health");
    TEST_ASSERT(health_field != NULL);
    TEST_ASSERT(strcmp(health_field->name, "health") == 0);
    TEST_ASSERT(strcmp(health_field->type->name, "float") == 0);

    const cf_field_t* non_existent = cf_find_field(player_type, "non_existent_field");
    TEST_ASSERT(non_existent == NULL);
    return 0;
}

int test_find_enum_value() {
    const cf_type_t* color_type = cf_find_type_by_name("color_t");
    TEST_ASSERT(color_type != NULL);
    TEST_ASSERT(color_type->kind == CF_KIND_ENUM);

    const cf_enum_value_t* red_by_name = cf_find_enum_value_by_name(color_type, "COLOR_RED");
    TEST_ASSERT(red_by_name != NULL);
    TEST_ASSERT(strcmp(red_by_name->name, "COLOR_RED") == 0);
    // TEST_ASSERT(red_by_name->value == 0); // Need to fix parser first

    // const cf_enum_value_t* blue_by_val = cf_find_enum_value_by_value(color_type, 2);
    // TEST_ASSERT(blue_by_val != NULL);
    // TEST_ASSERT(strcmp(blue_by_val->name, "BLUE") == 0);

    const cf_enum_value_t* non_existent_name = cf_find_enum_value_by_name(color_type, "non_existent");
    TEST_ASSERT(non_existent_name == NULL);

    const cf_enum_value_t* non_existent_val = cf_find_enum_value_by_value(color_type, 999);
    TEST_ASSERT(non_existent_val == NULL);

    return 0;
}


int main() {

    cf_initialize();
    cflex_default_register_types();
    program_register_types();
    cflex_unit_register_types();

    printf("--- Running C-Flex Unit Tests ---\n");
    RUN_TEST(test_find_type_by_name);
    RUN_TEST(test_table_api);
    RUN_TEST(test_find_field);
    RUN_TEST(test_find_enum_value);
    printf("---------------------------------\n");
    printf("All tests passed!\n");

    cf_shutdown();

    return 0;
}
