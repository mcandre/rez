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
    bool list_tasks = false,
         reload = false,
         debug = false;

    int i;
    for (i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "-l") == 0) {
            list_tasks = true;
            continue;
        }

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

    // const int task_len = argc - i;
    // const char **tasks = argv + i;

    if (list_tasks) {
        fprintf(stderr, "unimplemented");
        return EXIT_FAILURE;
    }

    char *compiler = REZ_DEFAULT_UNIX_COMPILER;

    bool windows = getenv(REZ_COMSPEC_ENV_VAR) != NULL;

    if (windows) {
        compiler = REZ_DEFAULT_WINDOWS_COMPILER;
    }

    char *compiler_override = getenv(REZ_COMPILER_CPP_ENV_VAR);

    if (compiler_override != NULL) {
        compiler = compiler_override;
    } else {
        compiler_override = getenv(REZ_COMPILER_C_ENV_VAR);

        if (compiler_override != NULL) {
            compiler = compiler_override;
        }
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
