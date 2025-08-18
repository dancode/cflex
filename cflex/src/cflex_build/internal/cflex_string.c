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
