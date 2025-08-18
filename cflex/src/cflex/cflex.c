#include "cflex.h"
#include <string.h>

// These global variables are defined in the generated source file and
// discovered by the linker.
extern const cf_type_t* cf_type_array[];
extern const int32_t cf_type_count;
// The generated header is not included here, so we don't know the enum type.
// We use a simple integer for the public API.
extern const int32_t CF_TYPE_ID_COUNT;


const cf_type_t* cf_find_type_by_name(const char* name)
{
    if (!name)
    {
        return NULL;
    }

    for (int32_t i = 0; i < cf_type_count; ++i)
    {
        const cf_type_t* type = cf_type_array[i];
        if (type && type->name && strcmp(name, type->name) == 0)
        {
            return type;
        }
    }

    return NULL;
}

const cf_type_t* cf_find_type_by_id(int32_t id)
{
    // We can't use the enum count directly, so we rely on cf_type_count
    // for bounds checking. A better implementation might have the count
    // in the generated file match exactly. For now, this is safe.
    if (id >= 0 && id < cf_type_count)
    {
        return cf_type_array[id];
    }
    return NULL;
}
