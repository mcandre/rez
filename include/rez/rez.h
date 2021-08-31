#include <stdbool.h>

#define REZ_VERSION "0.0.1"

#define REZ_FILE_CPP "rez.cpp"
#define REZ_FILE_C "rez.c"
#define REZ_CACHE_DIR ".rez"
#define REZ_CACHE_FILE "rez-env.txt"
#define REZ_CACHE_ARTIFACT_SUB_DIR "bin"
#define REZ_ARTIFACT_UNIX "rez"
#define REZ_ARTIFACT_WINDOWS "rez.exe"
#define REZ_PATH_SEP_UNIX "/"
#define REZ_PATH_SEP_WINDOWS "\\"
#define REZ_DEFAULT_COMPILER_UNIX_CPP "c++"
#define REZ_DEFAULT_COMPILER_UNIX_C "cc"
#define REZ_DEFAULT_COMPILER_WINDOWS "cl"
#define REZ_COMPILER_ENV_VAR_CPP "CXX"
#define REZ_COMPILER_ENV_VAR_C "CC"

#define REZ_COMSPEC_ENV_VAR "COMSPEC"

#define REZ_VCVARS_SCRIPT "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\vcvarsall.bat"

// https://devblogs.microsoft.com/oldnewthing/20100203-00/?p=15083
#define REZ_MAX_ENV_BLOCK 32760

typedef enum {
    REZ_LANG_CPP,
    REZ_LANG_C
} rez_lang_t;

typedef struct {
    bool debug;
    bool windows;
    bool msvc;
    char *rez_file;
    rez_lang_t rez_lang;
    char *compiler;
    char *rez_artifact;
} rez_config;

void rez_dump_config(const rez_config *config);

int rez_clean_cache_file(char *path);

bool rez_detect_windows(void);

void rez_path_join(char *buffer, char *parent_dir, char *child_file, const rez_config *config);

void rez_artifact(char *buffer, const rez_config *config);

void rez_cache(char *buffer, const rez_config *config);

int rez_apply_msvc_toolchain(rez_config *config);

int rez_load_config(rez_config *config);
