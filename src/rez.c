#include <errno.h>

#include <stdio.h>

#if defined(_MSC_VER)
#define pclose _pclose
#define popen _popen
#define S_ISDIR(x) ((x) & _S_IFDIR)
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "rez/rez.h"

void rez_dump_config(const rez_config *config) {
    printf(
        "{ debug: %d, windows: %d, msvc: %d, rez_file: %s, rez_lang: %d, compiler: %s, rez_artifact: %s }",
        config->debug,
        config->windows,
        config->msvc,
        config->rez_file,
        config->rez_lang,
        config->compiler,
        config->rez_artifact
    );
}

int rez_clean_cache_file(char *path) {
    struct stat buffer;
    const int status = stat(path, &buffer);

    errno = 0;
    switch (status) {
    case 0:
        if (S_ISDIR(buffer.st_mode)) {
            fprintf(stderr, "unimplemented\n");

            // ...

            return -1;
        }

        errno = 0;
        if (remove(path) != 0) {
            fprintf(stderr, "error removing file: %s\n", path);
            return -1;
        }
    case ENOENT:
        return 0;
    default:
        fprintf(stderr, "encountered error querying path %s: %d\n", path, status);
        return -1;
    }
}

bool rez_detect_windows(void) {
    return getenv(REZ_COMSPEC_ENV_VAR) != NULL;
}

void rez_path_join(char *buffer, char *parent_dir, char *child_file, const rez_config *config) {
    strcat(buffer, parent_dir);

    if (config->windows) {
        strcat(buffer, REZ_PATH_SEP_WINDOWS);
    } else {
        strcat(buffer, REZ_PATH_SEP_UNIX);
    }

    strcat(buffer, child_file);
}

void rez_artifact(char *buffer, const rez_config *config) {
    char *executable;

    if (config->windows) {
        executable = REZ_ARTIFACT_WINDOWS;
    } else {
        executable = REZ_ARTIFACT_UNIX;
    }

    rez_path_join(buffer, REZ_CACHE_DIR, executable, config);
}

void rez_cache(char *buffer, const rez_config *config) {
    rez_path_join(buffer, REZ_CACHE_DIR, REZ_CACHE_FILE, config);
}

int rez_apply_msvc_toolchain(rez_config *config) {
    struct stat buffer_cache_dir,
                buffer_cache_path;

    errno = 0;
    const bool cache_dir_exists = stat(REZ_CACHE_DIR, &buffer_cache_dir) == 0;

    if (!cache_dir_exists) {
        if (errno == ENOENT) {
            errno = 0;
            if (mkdir(REZ_CACHE_DIR) != 0) {
                fprintf(stderr, "error creating cache directory %s. errno: %d\n", REZ_CACHE_DIR, errno);
                return -1;
            }
        } else {
            fprintf(stderr, "unable to query cache directory %s. errno: %d\n", errno);
        }
    }

    char cache_path[1024];
    rez_cache(cache_path, config);

    errno = 0;
    bool cache_path_exists = stat(cache_path, &buffer_cache_path) == 0;

    if (!cache_path_exists && errno != ENOENT) {
        fprintf(stderr, "unable to query file path %s. errno: %d\n", cache_path, errno);
        return -1;
    }

    FILE *cache;
    char line[REZ_MAX_ENV_BLOCK];

    if (!cache_path_exists) {
        fprintf(stderr, "querying msvc toolchain...\n");

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
        cache = fopen(cache_path, "w");

        if (cache == NULL) {
            fprintf(stderr, "unable to open cache file for writing: %s\n", cache_path);
            return -1;
        }

        errno = 0;
        FILE *process = popen(command, "r");

        if (process == NULL) {
            return -1;
        }

        while (fgets(line, sizeof(line), process) != NULL) {
            errno = 0;
            if (fputs(line, cache) == EOF) {
                fprintf(stderr, "error writing to cache file: %s\n", cache_path);
                return -1;
            }
        }

        if (fclose(cache) == EOF) {
            fprintf(stderr, "error closing cache file: %s\n", cache_path);
            return -1;
        }

        if (pclose(process) != EXIT_SUCCESS) {
            fprintf(stderr, "error closing process handle\n");
            return -1;
        }
    }

    errno = 0;
    cache = fopen(cache_path, "r");

    if (cache == NULL) {
        fprintf(stderr, "error reading from cache file: %s\n", cache_path);
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
        fprintf(stderr, "error closing cache file: %s\n", cache_path);
        return -1;
    }

    return 0;
}

int rez_load_config(rez_config *config) {
    config->windows = rez_detect_windows();
    struct stat buffer_cpp;

    errno = 0;
    const int status_cpp = stat(REZ_FILE_CPP, &buffer_cpp);
    if (status_cpp == 0) {
        config->rez_file = REZ_FILE_CPP;
        config->rez_lang = REZ_LANG_CPP;
    } else if (errno == ENOENT) {
        struct stat buffer_c;
        errno = 0;
        const int status_c = stat(REZ_FILE_C, &buffer_c);

        if (status_c == 0) {
            config->rez_file = REZ_FILE_C;
            config->rez_lang = REZ_LANG_C;
        } else if (errno == ENOENT) {
            fprintf(stderr, "missing rez file rez.c[pp]\n");
            return -1;
        } else {
            fprintf(stderr, "unable to query file path %s. errno: %d\n", REZ_FILE_C, errno);
            return -1;
        }
    } else {
        fprintf(stderr, "unable to query file path %s. errno: %d\n", REZ_FILE_CPP, errno);
        return -1;
    }

    if (config->windows) {
        config->compiler = REZ_DEFAULT_COMPILER_WINDOWS;
        config->msvc = true;
    } else if (config->rez_lang == REZ_LANG_CPP) {
        config->compiler = REZ_DEFAULT_COMPILER_UNIX_CPP;
    } else {
        config->compiler = REZ_DEFAULT_COMPILER_UNIX_C;
    }

    char *compiler_override = NULL;

    switch (config->rez_lang) {
    case REZ_LANG_CPP:
        compiler_override = getenv(REZ_COMPILER_ENV_VAR_CPP);

        if (compiler_override != NULL) {
            config->compiler = compiler_override;
        }

        break;
    default:
        compiler_override = getenv(REZ_COMPILER_ENV_VAR_C);

        if (compiler_override != NULL) {
            config->compiler = compiler_override;
        }
    }

    if (config->msvc && rez_apply_msvc_toolchain(config) < 0) {
        fprintf(stderr, "error applying msvc toolchain\n");
        return -1;
    }

    return 0;
}
