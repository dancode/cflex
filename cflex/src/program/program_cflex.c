// This file is the unity build container for the cflex library for the 'program' executable.
// It includes the library implementation and all necessary generated reflection data.

// Include the generated C files for the modules this executable needs.
// This compiles the reflection data directly into this translation unit.
#include "program_generated.c"

#include "cflex_implementation.c"    // this is the same as c file.
