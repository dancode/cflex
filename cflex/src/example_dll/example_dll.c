#include "example_dll.h"
#include "cflex.h"
#include "example_dll_generated.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

// Define a function pointer type for the register_types function
typedef void (*register_types_func)(void);

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

void* load_library(const char* lib_name) {
    void* handle = NULL;
#ifdef _WIN32
    handle = LoadLibrary(lib_name);
#else
    handle = dlopen(lib_name, RTLD_LAZY);
#endif

    if (!handle) {
        printf("Error: Could not load library %s\n", lib_name);
#ifndef _WIN32
        printf("dlerror: %s\n", dlerror());
#endif
    }
    return handle;
}

register_types_func get_register_func(void* handle, const char* func_name) {
    void* func_ptr = NULL;
#ifdef _WIN32
    func_ptr = (void*)GetProcAddress(handle, func_name);
#else
    func_ptr = dlsym(handle, func_name);
#endif

    if (!func_ptr) {
        printf("Error: Could not find function %s\n", func_name);
#ifndef _WIN32
        printf("dlerror: %s\n", dlerror());
#endif
        return NULL;
    }

    register_types_func func;
    memcpy(&func, &func_ptr, sizeof(func));
    return func;
}

void unload_library(void* handle) {
#ifdef _WIN32
    if (handle) FreeLibrary((HMODULE)handle);
#else
    if (handle) dlclose(handle);
#endif
}


int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    cf_initialize();
    example_dll_register_types();

    printf("C-Flex Dynamic Library Reflection Demo\n");
    printf("--------------------------------------\n");

#ifdef _WIN32
    const char* mod1_path = "example_dll_module_1.dll";
    const char* mod2_path = "example_dll_module_2.dll";
#else
    const char* mod1_path = "libexample_dll_module_1.so";
    const char* mod2_path = "libexample_dll_module_2.so";
#endif

    void* mod1_handle = load_library(mod1_path);
    void* mod2_handle = load_library(mod2_path);

    if (mod1_handle) {
        register_types_func mod1_register = get_register_func(mod1_handle, "example_dll_module_1_register_types");
        if (mod1_register) {
            printf("Registering types from module 1...\n");
            mod1_register();
        }
    }

    if (mod2_handle) {
        register_types_func mod2_register = get_register_func(mod2_handle, "example_dll_module_2_register_types");
        if (mod2_register) {
            printf("Registering types from module 2...\n");
            mod2_register();
        }
    }

    print_all_types();

    // Find types from both modules
    printf("Finding type 'dll_mod1_struct_t' from dynamic library 1...\n");
    const cf_type_t* mod1_type = cf_find_type_by_name("dll_mod1_struct_t");
    print_type_details(mod1_type);
    printf("\n");

    printf("Finding type 'dll_mod2_struct_t' from dynamic library 2...\n");
    const cf_type_t* mod2_type = cf_find_type_by_name("dll_mod2_struct_t");
    print_type_details(mod2_type);
    printf("\n");

    if (mod1_handle) unload_library(mod1_handle);
    if (mod2_handle) unload_library(mod2_handle);

    cf_shutdown();
    return 0;
}
