#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rez/rez.h"

void usage(const char *program) {
    fprintf(stderr, "usage: %s [OPTION] [<task> [<task> [<task>...]]]\n\n", program);

    fprintf(stderr, "-l\tList available tasks\n");
    fprintf(stderr, "-r\tForce reload cache\n");
    fprintf(stderr, "-d\tEnable debugging information\n");
    fprintf(stderr, "-v\tShow version information\n");
    fprintf(stderr, "-h\tShow usage information\n");
}

int main(int argc, const char **argv) {
    bool reload = false,
         debug = false;

    int i;
    for (i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "-r") == 0) {
            reload = true;
            continue;
        }

        if (strcmp(arg, "-d") == 0) {
            debug = true;
            continue;
        }

        if (strcmp(arg, "-v") == 0) {
            printf("rez %s\n", REZ_VERSION);
            return EXIT_SUCCESS;
        }

        if (strcmp(arg, "-h") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        }

        break;
    }

    // const char **rest = argv + i;

    char *rez_path = REZ_FILE_CPP;

    int status = rez_file_status(REZ_FILE_CPP);

    if (status != 0) {
        if (status != ENOENT) {
            fprintf(stderr, "unable to query file path: %s\n", REZ_FILE_CPP);
            return EXIT_FAILURE;
        }

        status = rez_file_status(REZ_FILE_C);

        switch (status) {
        case 0:
            rez_path = REZ_FILE_C;
            break;
        case ENOENT:
            fprintf(stderr, "missing rez file rez.c[pp]\n");
            return EXIT_FAILURE;
        default:
            fprintf(stderr, "unable to query file path: %s\n", REZ_FILE_C);
            return EXIT_FAILURE;
        }
    }

    char *compiler = REZ_DEFAULT_COMPILER_UNIX_CPP;

    if (strcmp(rez_path, REZ_FILE_C) == 0) {
        compiler = REZ_DEFAULT_COMPILER_UNIX_C;
    }

    bool windows = getenv(REZ_COMSPEC_ENV_VAR) != NULL;

    if (windows) {
        compiler = REZ_DEFAULT_COMPILER_WINDOWS;
    }

    if (debug) {
        fprintf(stderr, "compiler: %s\n", compiler);
    }

    if (windows && rez_load_msvc(reload) < 0) {
        fprintf(stderr, "error loading msvc\n");
        return EXIT_FAILURE;
    }

    // ...

    return EXIT_SUCCESS;
}
