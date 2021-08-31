#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "rez/rez.h"

void usage(const char *program) {
    fprintf(stderr, "usage: %s [OPTION] [<task> [<task> [<task>...]]]\n\n", program);

    fprintf(stderr, "-l\tList available tasks\n");
    fprintf(stderr, "-c\tClean rez caches\n");
    fprintf(stderr, "-d\tEnable debugging information\n");
    fprintf(stderr, "-v\tShow version information\n");
    fprintf(stderr, "-h\tShow usage information\n");
}

void banner(void) {
    printf("rez %s\n", REZ_VERSION);
}

int main(int argc, const char **argv) {
    rez_config config = { 0 };

    int i;
    for (i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "-c") == 0) {
            fprintf(stderr, "unimplemented\n");
            return EXIT_FAILURE;
        }

        if (strcmp(arg, "-d") == 0) {
            config.debug = true;
            continue;
        }

        if (strcmp(arg, "-v") == 0) {
            banner();
            return EXIT_SUCCESS;
        }

        if (strcmp(arg, "-h") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        }

        break;
    }

    // const char **rest = argv + i;

    if (rez_load_config(&config) < 0) {
        fprintf(stderr, "error loading config\n");
        return EXIT_FAILURE;
    }

    if (config.debug) {
        rez_dump_config(&config);
    }

    char artifact[1024] = { 0 };
    rez_artifact(artifact, &config);

    struct stat artifact_buffer;

    errno = 0;
    const int artifact_exists = stat(artifact, &artifact_buffer) == 0;

    if (!artifact_exists) {
        if (errno != ENOENT) {
            fprintf(stderr, "unable to query artifact path %s. errno: %d\n", artifact, errno);
            return EXIT_FAILURE;
        }

        char command[1024] = { 0 };
        strcat(command, config.compiler);
        strcat(command, " ");

        if (config.msvc) {
            // ...

            strcat(command, "/link /out ");
            strcat(command, artifact);
        } else {
            strcat(command, "-o ");
            strcat(command, artifact);

            // ...
        }

        fprintf(stderr, "running build command: %s\n", command);

        // ...
    }

    // ...

    return EXIT_SUCCESS;
}
