#include <errno.h>

#include <stdio.h>

#if defined(_MSC_VER)
#define pclose _pclose
#define popen _popen
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "rez/rez.h"

int rez_file_status(const char *path) {
    struct stat buffer;

    errno = 0;
    if (stat(path, &buffer) != 0) {
        return errno;
    }

    return 0;
}

bool rez_is_windows() {
    return getenv(REZ_COMSPEC_ENV_VAR) != NULL;
}

int rez_load_msvc(bool reload) {
    int cache_status = rez_file_status(REZ_CACHE_FILENAME);
    bool cache_exists = cache_status == 0;

    if (!cache_exists && errno != ENOENT) {
        fprintf(stderr, "unable to query file path: %s\n", REZ_CACHE_FILENAME);
        return -1;
    }

    FILE *cache;
    char line[REZ_MAX_ENV_BLOCK];

    if (reload || !cache_exists) {
        fprintf(stderr, "updating toolchain cache...\n");

        char command[1024];

        int result = sprintf(
            command,
            "powershell.exe -Command \"cmd /c \"\"%s\"\"\" & set",
            REZ_VCVARS_SCRIPT
        );

        if (result < 0) {
            fprintf(stderr, "encoding error\n");
            return result;
        }

        errno = 0;
        FILE *process = popen(command, "r");

        if (process == NULL) {
            return -1;
        }

        errno = 0;
        cache = fopen(REZ_CACHE_FILENAME, "w");

        if (cache == NULL) {
            fprintf(stderr, "unable to open cache file for writing: %s\n", REZ_CACHE_FILENAME);
            return -1;
        }

        while (fgets(line, sizeof(line), process) != NULL) {
            errno = 0;
            if (fputs(line, cache) == EOF) {
                fprintf(stderr, "error writing to cache file: %s\n", REZ_CACHE_FILENAME);
                return -1;
            }
        }

        if (fclose(cache) == EOF) {
            fprintf(stderr, "error closing cache file: %s\n", REZ_CACHE_FILENAME);
            return -1;
        }

        if (pclose(process) != EXIT_SUCCESS) {
            fprintf(stderr, "error closing process handle\n");
            return -1;
        }
    }

    errno = 0;
    cache = fopen(REZ_CACHE_FILENAME, "r");

    if (cache == NULL) {
        fprintf(stderr, "error reading from cache file: %s\n", REZ_CACHE_FILENAME);
        return -1;
    }

    while (fgets(line, sizeof(line), cache) != NULL) {
        // chomp
        line[strcspn(line, "\n")] = '\0';

#if defined(_MSC_VER)
        if (_putenv(line) != 0) {
#else
        if (putenv(line) != 0) {
#endif
            fprintf(stderr, "error applying environment variable key=value pair: %s\n", line);
            return -1;
        }
    }

    if (fclose(cache) == EOF) {
        fprintf(stderr, "error closing cache file: %s\n", REZ_CACHE_FILENAME);
        return -1;
    }

    return 0;
}
