#ifndef CFLEX_H
#define CFLEX_H

#include <stdint.h>

/**
 * @brief A structure to hold basic information about a reflected type.
 * Adheres to the project convention of using a _t suffix for type names.
 */
typedef struct cflex_type_info_t {
    const char* name;
    uint32_t size;
} cflex_type_info_t;

/**
 * @brief Initializes the cflex reflection library.
 * Adheres to the project convention of using lowercase function names.
 */
void cflex_init(void);

/**
 * @brief Retrieves type information for a given type name.
 * @param name The name of the type to look up.
 * @return A pointer to the type information structure, or NULL if not found.
 */
const cflex_type_info_t* cflex_get_type_by_name(const char* name);

#endif // CFLEX_H
