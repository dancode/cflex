#ifndef CFLEX_INTERNAL_H
#define CFLEX_INTERNAL_H

#include "../cflex.h"

// This function is intended for use by the generated code only.
// It registers a table of type pointers with the cflex runtime.
void cf_register_type_table(const cf_type_t* types[], int32_t count);

#endif // CFLEX_INTERNAL_H
