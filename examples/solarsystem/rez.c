#include <limits.h>
#if defined(__APPLE__)
#include <sys/syslimits.h>
#elif defined(_MSC_VER)
#include <windows.h>
#define PATH_MAX MAX_PATH
#endif

#if defined(_MSC_VER)
#include <fileapi.h>
#include <shellapi.h>
#pragma comment(lib, "shell32")
#else
#define _XOPEN_SOURCE 500
#define __USE_XOPEN_EXTENDED 1
#include <dirent.h>
#include <ftw.h>
#include <unistd.h>
#endif

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#if !defined(_MSC_VER)
static int remove_cb(const char *path, __attribute__((unused)) const struct stat *sb, __attribute__((unused)) int typeflag, __attribute__((unused)) struct FTW *ftwbuf) {
    errno = 0;
    const int status = remove(path);

    if (status) {
        fprintf(stderr, "error: unable to remove path: %s\n", path);
    }

    return status;
}
#endif

static int remove_all(const char *path) {
    int status;

#if defined(_MSC_VER)
    const DWORD buf = GetFileAttributesA(path);

    if (buf == INVALID_FILE_ATTRIBUTES) {
        return EXIT_SUCCESS;
    }

    // Double null terminated per SHFileOperation requirements.
    char *paths = calloc(strlen(path) + 2, sizeof(char));
    strcpy(paths, path);

    SHFILEOPSTRUCT shfo = {
        NULL,
        FO_DELETE,
        paths,
        NULL,
        FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION,
        FALSE,
        NULL,
        NULL
    };
    status = SHFileOperationA(&shfo);
    free(paths);

    if (status) {
        fprintf(stderr, "error: unable to remove path: %s\n", path);
    }

    return status;
#else
    struct stat buf;

    errno = 0;
    status = stat(path, &buf);

    if (status == 0) {
        if (S_ISREG(buf.st_mode)) {
            errno = 0;
            status = remove(path);

            if (status) {
                fprintf(stderr, "error: unable to remove path: %s\n", path);
            }

            return status;
        }

        errno = 0;
        return nftw(path, remove_cb, 64, FTW_DEPTH | FTW_PHYS);
    }

    if (errno == ENOENT) {
        return EXIT_SUCCESS;
    }

    fprintf(stderr, "error: unable to query for path: %s\n", path);
    return EXIT_FAILURE;
#endif
}

static void remove_all_abortable(const char *path) {
    if (remove_all(path) != 0) {
        exit(EXIT_FAILURE);
    }
}

static int cmake_init(void) {
    return system("cmake -B build .");
}

static int build(void) {
    const int status = cmake_init();

    if (status != EXIT_SUCCESS) {
        return status;
    }

    return system("cmake --build build --config Release");
}

static int install(void) {
    const int status = build();

    if (status != EXIT_SUCCESS) {
        return status;
    }

    return system("cmake --build build --target install");
}

static int uninstall(void) {
    const int status = cmake_init();

    if (status != EXIT_SUCCESS) {
        return status;
    }

    return system("cmake --build build --target uninstall");
}

static int run(void) {
    const int status = install();

    if (status != EXIT_SUCCESS) {
        return status;
    }

    return system("solarsystem");
}

static int clean_bin(void) {
    return remove_all("bin");
}

static int clean_cmake(void) {
    remove_all_abortable("build");
    return EXIT_SUCCESS;
}

static int clean(void) {
    clean_bin();
    clean_cmake();
    return EXIT_SUCCESS;
}

int main(int argc, const char **argv) {
    if (argc == 0) {
        fprintf(stderr, "error: missing program name\n");
        return EXIT_FAILURE;
    }

    const char *task_names[] = {
        "clean",
        "clean_bin",
        "clean_cmake",
        "cmake_init",
        "build",
        "install",
        "run",
        "uninstall"
    };
    const size_t task_sz = sizeof(task_names) / sizeof(char *);
    int (*task_functions[])(void) = {
        &clean,
        &clean_bin,
        &clean_cmake,
        &cmake_init,
        &build,
        &install,
        &run,
        &uninstall
    };
    int (*default_task)(void) = run;

    if (argc == 1) {
        if (default_task()) {
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    if (strcmp(argv[1], "-l") == 0) {
        for (size_t i = 0; i < task_sz; i++) {
            printf("%s\n", task_names[i]);
        }

        return EXIT_SUCCESS;
    }

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        bool found_task = false;

        for (size_t j = 0; j < task_sz; j++) {
            if (strcmp(arg, task_names[j]) == 0) {
                found_task = true;

                if (task_functions[j]()) {
                    return EXIT_FAILURE;
                }
            }
        }

        if (!found_task) {
            fprintf(stderr, "error: no such task: %s\n", arg);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
