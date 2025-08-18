#ifndef PROGRAM_H
#define PROGRAM_H

#include "cflex/cflex_macros.h"
#include <stdint.h>

// Note: In a real project, you might include a base vector library header instead.
CF_STRUCT()
typedef struct vec2_t {
    CF_FIELD() float x;
    CF_FIELD() float y;
} vec2_t;

CF_STRUCT()
typedef struct vec3_t {
    CF_FIELD() float x;
    CF_FIELD() float y;
    CF_FIELD() float z;
} vec3_t;

CF_STRUCT()
typedef struct player_t {
    CF_FIELD() int32_t power;
    CF_FIELD() float health;
    CF_FIELD() vec3_t position;
} player_t;

CF_ENUM()
typedef enum color_t
{
    COLOR_RED = 0,
    COLOR_GREEN,
    COLOR_BLUE
} color_t;

#endif // PROGRAM_H
