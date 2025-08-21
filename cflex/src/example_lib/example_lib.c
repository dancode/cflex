#include "example_lib.h"
#include "cflex.h"
#include "example_lib_generated.h"
#include "example_lib_module_1_generated.h"

#include <stdio.h>

// Forward declaration for the registration function from the static library module.
void example_lib_module_1_register_types(void);

void print_type_details(const cf_type_t* type) {
    if (!type) {
        printf("Type not found.\n");
        return;
    }
    printf("Details for '%s':\n", type->name);
    printf("  - Kind: %d\n", type->kind);
    printf("  - Size: %d bytes\n", type->size);
    printf("  - Alignment: %d bytes\n", type->align);
    if (type->kind == CF_KIND_STRUCT) {
        printf("  - Fields (%d):\n", type->struct_count);
        for (int32_t i = 0; i < type->struct_count; ++i) {
            const cf_field_t* field = &type->struct_array[i];
            printf("    - %s (type: %s, offset: %d)\n", field->name, field->type->name, field->offset);
        }
    }
}

void print_all_types() {
    printf("--- All Registered Types ---\n");
    int32_t num_tables = cf_get_num_tables();
    for (int32_t i = 0; i < num_tables; ++i) {
        const cf_type_t** types;
        int32_t           count;
        cf_get_table(i, &types, &count);
        printf("--- Table %d (%d types) ---\n", i, count);
        for (int32_t j = 0; j < count; ++j) {
            print_type_details(types[j]);
            printf("\n");
        }
    }
    printf("---------------------------------\n");
}

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    cf_initialize();

    // Register types from the main executable and the static library
    example_lib_register_types();
    example_lib_module_1_register_types();

    printf("C-Flex Static Library Reflection Demo\n");
    printf("-------------------------------------\n");

    print_all_types();

    printf("Finding type 'mod1_struct_t' from static library...\n");
    const cf_type_t* mod1_type = cf_find_type_by_name("mod1_struct_t");
    print_type_details(mod1_type);
    printf("\n");

    cf_shutdown();
    return 0;
}
