#ifndef MODULE_1_H
#define MODULE_1_H

#include "cflex_macros.h"
#include <stdint.h>

#if defined(_WIN32) && defined(BUILD_SHARED_LIBS)
    #ifdef MODULE_1_EXPORTS
        #define MODULE_1_API __declspec(dllexport)
    #else
        #define MODULE_1_API __declspec(dllimport)
    #endif
#else
    #define MODULE_1_API
#endif

CF_STRUCT()
typedef struct dll_mod1_struct_t {
    CF_FIELD() int32_t x;
    CF_FIELD() int32_t y;
} dll_mod1_struct_t;

#endif // MODULE_1_H
