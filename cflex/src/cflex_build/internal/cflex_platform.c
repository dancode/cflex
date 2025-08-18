// This file is intended for platform-specific implementations and is
// included directly into cflex_build.c for a unity build.
// For example, file system operations that differ between Windows and Linux
// would be implemented here using #ifdefs.

#include <stdio.h>

#ifdef _WIN32
#    define PLATFORM_NAME "Windows"
#else
#    define PLATFORM_NAME "Linux/Other"
#endif

// Example of a function that could have platform-specific implementations.
void
print_platform( void )
{
    printf( "Build tool is running on platform: %s\n", PLATFORM_NAME );
}
