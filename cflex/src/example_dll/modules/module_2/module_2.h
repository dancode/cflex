#ifndef MODULE_2_H
#define MODULE_2_H

#include "cflex_macros.h"
#include <stdint.h>

#if defined(_WIN32) && defined(BUILD_SHARED_LIBS)
    #ifdef MODULE_2_EXPORTS
        #define MODULE_2_API __declspec(dllexport)
    #else
        #define MODULE_2_API __declspec(dllimport)
    #endif
#else
    #define MODULE_2_API
#endif

CF_STRUCT()
typedef struct dll_mod2_struct_t {
    CF_FIELD() int32_t name_placeholder;
    CF_FIELD() double      value;
} dll_mod2_struct_t;

#endif // MODULE_2_H
