#include "cflex.h"
#include "program_types.h"
#include <stddef.h> // For offsetof

// -- Forward declarations for all type definitions --
// Primitives
static const cf_type_t cf_type_void;
static const cf_type_t cf_type_bool;
static const cf_type_t cf_type_char;
static const cf_type_t cf_type_i8;
static const cf_type_t cf_type_i16;
static const cf_type_t cf_type_i32;
static const cf_type_t cf_type_i64;
static const cf_type_t cf_type_u8;
static const cf_type_t cf_type_u16;
static const cf_type_t cf_type_u32;
static const cf_type_t cf_type_u64;
static const cf_type_t cf_type_f32;
static const cf_type_t cf_type_f64;
static const cf_type_t cf_type_cstr;

// User Types
static const cf_type_t cf_type_vec2_t;
static const cf_type_t cf_type_vec3_t;
static const cf_type_t cf_type_player_t;
static const cf_type_t cf_type_color_t;


// -- Generated Primitive Types --
static const cf_type_t cf_type_void = { .name = "void", .kind = CF_KIND_PRIMITIVE, .size = 0, .align = 0, .prim = CF_PRIM_VOID };
static const cf_type_t cf_type_bool = { .name = "bool", .kind = CF_KIND_PRIMITIVE, .size = sizeof(bool), .align = _Alignof(bool), .prim = CF_PRIM_BOOL };
static const cf_type_t cf_type_char = { .name = "char", .kind = CF_KIND_PRIMITIVE, .size = sizeof(char), .align = _Alignof(char), .prim = CF_PRIM_CHAR };
static const cf_type_t cf_type_i8 = { .name = "int8_t", .kind = CF_KIND_PRIMITIVE, .size = sizeof(int8_t), .align = _Alignof(int8_t), .prim = CF_PRIM_I8 };
static const cf_type_t cf_type_i16 = { .name = "int16_t", .kind = CF_KIND_PRIMITIVE, .size = sizeof(int16_t), .align = _Alignof(int16_t), .prim = CF_PRIM_I16 };
static const cf_type_t cf_type_i32 = { .name = "int32_t", .kind = CF_KIND_PRIMITIVE, .size = sizeof(int32_t), .align = _Alignof(int32_t), .prim = CF_PRIM_I32 };
static const cf_type_t cf_type_i64 = { .name = "int64_t", .kind = CF_KIND_PRIMITIVE, .size = sizeof(int64_t), .align = _Alignof(int64_t), .prim = CF_PRIM_I64 };
static const cf_type_t cf_type_u8 = { .name = "uint8_t", .kind = CF_KIND_PRIMITIVE, .size = sizeof(uint8_t), .align = _Alignof(uint8_t), .prim = CF_PRIM_U8 };
static const cf_type_t cf_type_u16 = { .name = "uint16_t", .kind = CF_KIND_PRIMITIVE, .size = sizeof(uint16_t), .align = _Alignof(uint16_t), .prim = CF_PRIM_U16 };
static const cf_type_t cf_type_u32 = { .name = "uint32_t", .kind = CF_KIND_PRIMITIVE, .size = sizeof(uint32_t), .align = _Alignof(uint32_t), .prim = CF_PRIM_U32 };
static const cf_type_t cf_type_u64 = { .name = "uint64_t", .kind = CF_KIND_PRIMITIVE, .size = sizeof(uint64_t), .align = _Alignof(uint64_t), .prim = CF_PRIM_U64 };
static const cf_type_t cf_type_f32 = { .name = "float", .kind = CF_KIND_PRIMITIVE, .size = sizeof(float), .align = _Alignof(float), .prim = CF_PRIM_F32 };
static const cf_type_t cf_type_f64 = { .name = "double", .kind = CF_KIND_PRIMITIVE, .size = sizeof(double), .align = _Alignof(double), .prim = CF_PRIM_F64 };
static const cf_type_t cf_type_cstr = { .name = "const char*", .kind = CF_KIND_PRIMITIVE, .size = sizeof(const char*), .align = _Alignof(const char*), .prim = CF_PRIM_CSTR };

// -- Generated Struct: vec2_t --
static const cf_field_t cf_vec2_t_fields[] = {
    { "x", &cf_type_f32, offsetof(vec2_t, x), 0 },
    { "y", &cf_type_f32, offsetof(vec2_t, y), 0 }
};
static const cf_type_t cf_type_vec2_t = { .name = "vec2_t", .kind = CF_KIND_STRUCT, .size = sizeof(vec2_t), .align = _Alignof(vec2_t), .struct_array = cf_vec2_t_fields, .struct_count = 2, .struct_parent = NULL, .struct_is_anonymous = false };

// -- Generated Struct: vec3_t --
static const cf_field_t cf_vec3_t_fields[] = {
    { "x", &cf_type_f32, offsetof(vec3_t, x), 0 },
    { "y", &cf_type_f32, offsetof(vec3_t, y), 0 },
    { "z", &cf_type_f32, offsetof(vec3_t, z), 0 }
};
static const cf_type_t cf_type_vec3_t = { .name = "vec3_t", .kind = CF_KIND_STRUCT, .size = sizeof(vec3_t), .align = _Alignof(vec3_t), .struct_array = cf_vec3_t_fields, .struct_count = 3, .struct_parent = NULL, .struct_is_anonymous = false };

// -- Generated Struct: player_t --
static const cf_field_t cf_player_t_fields[] = {
    { "power", &cf_type_i32, offsetof(player_t, power), 0 },
    { "health", &cf_type_f32, offsetof(player_t, health), 0 },
    { "position", &cf_type_vec3_t, offsetof(player_t, position), 0 }
};
static const cf_type_t cf_type_player_t = { .name = "player_t", .kind = CF_KIND_STRUCT, .size = sizeof(player_t), .align = _Alignof(player_t), .struct_array = cf_player_t_fields, .struct_count = 3, .struct_parent = NULL, .struct_is_anonymous = false };

// -- Generated Enum: color_t --
static const cf_enum_value_t cf_color_t_values[] = {
    { "COLOR_RED", COLOR_RED }, { "COLOR_GREEN", COLOR_GREEN }, { "COLOR_BLUE", COLOR_BLUE }
};
static const cf_type_t cf_type_color_t = { .name = "color_t", .kind = CF_KIND_ENUM, .size = sizeof(color_t), .align = _Alignof(color_t), .enum_array = cf_color_t_values, .enum_count = 3, .enum_is_bitflag = false };

// -- Global Type Table --
const cf_type_t* cf_type_array[] = {
    // Primitives
    &cf_type_void,
    &cf_type_bool,
    &cf_type_char,
    &cf_type_i8,
    &cf_type_i16,
    &cf_type_i32,
    &cf_type_i64,
    &cf_type_u8,
    &cf_type_u16,
    &cf_type_u32,
    &cf_type_u64,
    &cf_type_f32,
    &cf_type_f64,
    &cf_type_cstr,
    // User types
    &cf_type_vec2_t,
    &cf_type_vec3_t,
    &cf_type_player_t,
    &cf_type_color_t,
};

const int32_t cf_type_count = sizeof(cf_type_array) / sizeof(cf_type_array[0]);
