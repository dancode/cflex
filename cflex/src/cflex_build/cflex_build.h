#ifndef CFLEX_BUILD_H
#define CFLEX_BUILD_H

#include <stdint.h>

// Using _t suffix for structure types as per conventions
typedef struct cflex_build_options_t
{
    const char* input_dir;
    const char* output_dir;
} cflex_build_options_t;

// Using lowercase function names as per conventions
void run_cflex_build( const cflex_build_options_t* options );

#endif    // CFLEX_BUILD_H
