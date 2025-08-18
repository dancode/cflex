#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Simple helper to check if a string ends with a suffix.
static bool ends_with(const char* str, const char* suffix) {
    if (!str || !suffix) {
        return false;
    }
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > str_len) {
        return false;
    }
    return strncmp(str + str_len - suffix_len, suffix, suffix_len) == 0;
}

void find_header_files(const char* path, file_list_t* header_files) {
    file_list_t all_files;
    if (!scan_directory(path, &all_files)) {
        header_files->count = 0;
        return;
    }

    header_files->count = 0;
    for (int i = 0; i < all_files.count; ++i) {
        if (ends_with(all_files.files[i], ".h")) {
            if (header_files->count < MAX_FILES) {
                // Ensure we don't overflow the buffer
                strncpy(header_files->files[header_files->count], all_files.files[i], MAX_PATH_LENGTH - 1);
                // Explicitly null-terminate
                header_files->files[header_files->count][MAX_PATH_LENGTH - 1] = '\0';
                header_files->count++;
            } else {
                fprintf(stderr, "Warning: Exceeded max header file limit of %d\n", MAX_FILES);
                break;
            }
        }
    }
}
