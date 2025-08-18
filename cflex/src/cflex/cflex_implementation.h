#ifndef CFLEX_IMPLEMENTATION_H
#define CFLEX_IMPLEMENTATION_H

#include "cflex.h"
#include <string.h>

// The generated source file is included here to provide the reflection data.
// Using angle brackets as a hint to the IDE to search the include paths.
#include <cflex_generated.c>

// Include internal implementation files (unity build style).
#include "internal/cflex_parser.h"

const cf_type_t *cf_find_type_by_name(const char *name) {
  if (!name) {
    return NULL;
  }

  for (int32_t i = 0; i < cf_type_count; ++i) {
    const cf_type_t *type = cf_type_array[i];
    if (type && type->name && strcmp(name, type->name) == 0) {
      return type;
    }
  }

  return NULL;
}

const cf_type_t *cf_find_type_by_id(cf_type_id_t id) {
  if (id >= 0 && id < CF_TYPE_ID_COUNT) {
    return cf_type_array[id];
  }
  return NULL;
}

#endif // CFLEX_IMPLEMENTATION_H
