#ifndef CFLEX_H
#define CFLEX_H

#include <stdbool.h>
#include <stdint.h>

// Forward declaration
struct cf_type_t;

// Kind of reflected type
typedef enum cf_kind_t
{
    CF_KIND_PRIMITIVE,
    CF_KIND_STRUCT,
    CF_KIND_ENUM,
} cf_kind_t;

// Built-in primitive types
typedef enum cf_prim_t
{
    CF_PRIM_VOID,
    CF_PRIM_BOOL,
    CF_PRIM_CHAR,
    CF_PRIM_I8,
    CF_PRIM_I16,
    CF_PRIM_I32,
    CF_PRIM_I64,
    CF_PRIM_U8,
    CF_PRIM_U16,
    CF_PRIM_U32,
    CF_PRIM_U64,
    CF_PRIM_F32,
    CF_PRIM_F64,
    CF_PRIM_CSTR,    // C-style string (const char*)
    CF_PRIM_COUNT
} cf_prim_t;

// Struct member information
typedef struct cf_field_t
{
    const char*             name;
    const struct cf_type_t* type;      // The field type
    const int32_t           offset;    // offsetof(struct, field)
    const int32_t           flags;     // For future metadata
} cf_field_t;

// Enum value information
typedef struct cf_enum_value_t
{
    const char* name;
    int32_t     value;
} cf_enum_value_t;

// The core reflection type, a discriminated union on "kind"
typedef struct cf_type_t
{
    const char* name;     // Type name (e.g., "int32_t", "player_t")
    cf_kind_t   kind;     // What kind of type this is
    int32_t     size;     // sizeof(type)
    int32_t     align;    // alignof(type)

    union
    {
        // CF_KIND_PRIMITIVE
        struct
        {
            cf_prim_t prim;
        };

        // CF_KIND_STRUCT
        struct
        {
            const struct cf_type_t*  struct_parent;
            const struct cf_field_t* struct_array;
            const int32_t            struct_count;
            const bool               struct_is_anonymous;
        };

        // CF_KIND_ENUM
        struct
        {
            const struct cf_enum_value_t* enum_array;
            const int32_t                 enum_count;
            const bool                    enum_is_bitflag;
        };
    };
} cf_type_t;

// Include the public generated header which defines cf_type_id_t
#include "cflex_generated.h"

// --- Library API ---

// Find a type by its name (e.g., "player_t").
const cf_type_t* cf_find_type_by_name( const char* name );

// Find a type by its generated ID.
const cf_type_t* cf_find_type_by_id( cf_type_id_t id );

// Get a pointer to the array of all registered types.
void cf_get_all_types( const cf_type_t*** out_types, int32_t* out_count );

// For a given struct type, find a field by its name.
const cf_field_t* cf_find_field( const cf_type_t* type, const char* name );

// For a given enum type, find a value by its name.
const cf_enum_value_t* cf_find_enum_value_by_name( const cf_type_t* type, const char* name );

// For a given enum type, find a value by its integer value.
const cf_enum_value_t* cf_find_enum_value_by_value( const cf_type_t* type, int32_t value );

#endif    // CFLEX_H
