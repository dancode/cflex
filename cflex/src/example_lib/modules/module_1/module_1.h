#ifndef MODULE_1_H
#define MODULE_1_H

#include "cflex_macros.h"
#include <stdint.h>

CF_STRUCT()
typedef struct mod1_struct_t {
    CF_FIELD() int32_t value;
    CF_FIELD() float   factor;
} mod1_struct_t;

#endif // MODULE_1_H
