#include <limits.h>
#if defined(__APPLE__)
#include <sys/syslimits.h>
#elif defined(_MSC_VER)
#include <windows.h>
#define PATH_MAX MAX_PATH
#endif

#define _XOPEN_SOURCE 500
#define __USE_XOPEN_EXTENDED 1
#include <ftw.h>
#include <stdio.h>
#include <unistd.h>

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

static int remove_cb(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    errno = 0;
    int status = remove(path);

    if (status) {
        fprintf(stderr, "error: unable to remove path: %s\n", path);
    }

    return status;
}

static int remove_all(const char *path) {
    struct stat buf;

    errno = 0;
    int status = stat(path, &buf);

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
}

static void remove_all_abortable(const char *path) {
    if (remove_all(path) != 0) {
        exit(EXIT_FAILURE);
    }
}

static int cmake_init() {
    return system("cmake .");
}

static int build() {
    const int status = cmake_init();

    if (status != EXIT_SUCCESS) {
        return status;
    }

    return system("cmake --build . --config Release");
}

static int install() {
    const int status = build();

    if (status != EXIT_SUCCESS) {
        return status;
    }

    return system("cmake --build . --target install");
}

static int uninstall() {
    const int status = cmake_init();

    if (status != EXIT_SUCCESS) {
        return status;
    }

    return system("cmake --build . --target uninstall");
}

static int run() {
    const int status = install();

    if (status != EXIT_SUCCESS) {
        return status;
    }

    return system("solarsystem");
}

static int clean_bin() {
    return remove_all("bin");
}

static int clean_junk_extensions_cb(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    const char *junk_extensions[] = {
        ".dir",
        ".filters",
        ".obj",
        ".sln",
        ".vcxproj"
    };
    const size_t junk_extensions_sz = sizeof(junk_extensions)/sizeof(char*);
    const char *extension = strchr(path, '.');

    if (extension) {
        for (size_t i = 0; i < junk_extensions_sz; i++) {
            if (extension && strcmp(extension, junk_extensions[i]) == 0) {
                int status = remove_all(path);

                if (status != 0) {
                    return status;
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

static int clean_msvc() {
    remove_all_abortable("x64");
    remove_all_abortable("x86");

    errno = 0;
    char *cwd = malloc(sizeof(char) * PATH_MAX);
    if (!getcwd(cwd, PATH_MAX)) {
        fprintf(stderr, "error: unable to access current working directory\n");
        free(cwd);
        return EXIT_FAILURE;
    }

    errno = 0;
    const int status = nftw(cwd, clean_junk_extensions_cb, 64, FTW_DEPTH | FTW_PHYS);
    free(cwd);
    return status;
}

static int clean_cmake() {
    remove_all_abortable("install_manifest.txt");
    remove_all_abortable("Makefile");
    remove_all_abortable("CMakeFiles");
    remove_all_abortable("CMakeCache.txt");
    remove_all_abortable("cmake_install.cmake");
    remove_all_abortable("CTestTestfile.cmake");
    return EXIT_SUCCESS;
}

static int clean() {
    clean_bin();
    clean_msvc();
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
        "clean_msvc",
        "cmake_init",
        "build",
        "install",
        "run",
        "uninstall"
    };
    const size_t task_sz = sizeof(task_names)/sizeof(char*);
    int (*task_functions[])() = {
        &clean,
        &clean_bin,
        &clean_cmake,
        &clean_msvc,
        &cmake_init,
        &build,
        &install,
        &run,
        &uninstall
    };
    int (*default_task)() = run;

    if (argc == 1) {
        return default_task();
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
                const int status = task_functions[j]();

                if (status != EXIT_SUCCESS) {
                    return status;
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
