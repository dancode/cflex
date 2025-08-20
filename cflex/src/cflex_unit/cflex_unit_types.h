#ifndef CFLEX_UNIT_TYPES_H
#define CFLEX_UNIT_TYPES_H

#include "cflex_macros.h"
#include <stdint.h>

CF_STRUCT()
typedef struct test_vec2_t
{
    CF_FIELD() float x;
    CF_FIELD() float y;
} test_vec2_t;

CF_ENUM()
typedef enum test_enum_t
{
    TEST_ENUM_A,
    TEST_ENUM_B,
    TEST_ENUM_C
} test_enum_t;

CF_STRUCT()
typedef struct test_struct_t
{
    CF_FIELD() int32_t a;
    CF_FIELD() test_vec2_t v;
    CF_FIELD() test_enum_t e;
} test_struct_t;

#endif // CFLEX_UNIT_TYPES_H
