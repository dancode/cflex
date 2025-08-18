#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

bool scan_directory(const char* path, file_list_t* file_list) {
    file_list->count = 0;

#ifdef _WIN32
    char search_path[MAX_PATH_LENGTH];
    snprintf(search_path, sizeof(search_path), "%s\\*", path);

    WIN32_FIND_DATAA find_data;
    HANDLE h_find = FindFirstFileA(search_path, &find_data);

    if (h_find == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error: Could not open directory %s\n", path);
        return false;
    }

    do {
        if (strcmp(find_data.cFileName, ".") != 0 && strcmp(find_data.cFileName, "..") != 0) {
            if (file_list->count < MAX_FILES) {
                snprintf(file_list->files[file_list->count], MAX_PATH_LENGTH, "%s\\%s", path, find_data.cFileName);
                file_list->count++;
            } else {
                fprintf(stderr, "Warning: Exceeded max file limit of %d\n", MAX_FILES);
                break;
            }
        }
    } while (FindNextFileA(h_find, &find_data) != 0);

    FindClose(h_find);
#else // POSIX
    DIR* dir = opendir(path);
    if (dir == NULL) {
        fprintf(stderr, "Error: Could not open directory %s\n", path);
        return false;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
             if (file_list->count < MAX_FILES) {
                snprintf(file_list->files[file_list->count], MAX_PATH_LENGTH, "%s/%s", path, entry->d_name);
                file_list->count++;
            } else {
                fprintf(stderr, "Warning: Exceeded max file limit of %d\n", MAX_FILES);
                break;
            }
        }
    }

    closedir(dir);
#endif

    return true;
}
