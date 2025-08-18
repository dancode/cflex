#include "cflex.h"
#include <string.h>

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

const cf_type_t* cf_find_type_by_id(cf_type_id_t id)
{
    if (id >= 0 && id < CF_TYPE_ID_COUNT)
    {
        return cf_type_array[id];
    }
    return NULL;
}
