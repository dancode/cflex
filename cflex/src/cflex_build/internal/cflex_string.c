#include <ctype.h>
#include <stdbool.h>

const char* ltrim(const char* str) {
    while (*str && isspace((unsigned char)*str)) {
        str++;
    }
    return str;
}

bool is_identifier_char(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

void str_copy(char* dst, const char* src, int32_t dst_size) {
    if (dst_size == 0) return;
    strncpy(dst, src, dst_size - 1);
    dst[dst_size - 1] = '\0';
}

void substr_copy(char* dst, const char* src, int32_t len, int32_t dst_size) {
    if (dst_size == 0) return;

    int32_t copy_len = len;
    if (copy_len >= dst_size) {
        copy_len = dst_size - 1;
    }

    memcpy(dst, src, copy_len);
    dst[copy_len] = '\0';
}

