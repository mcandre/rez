#include <stdbool.h>

#define REZ_VERSION "0.0.1"

#define REZ_FILE_CPP "rez.cpp"

#define REZ_FILE_C "rez.c"

#define REZ_DEFAULT_COMPILER_UNIX_CPP "c++"

#define REZ_DEFAULT_COMPILER_UNIX_C "cc"

#define REZ_DEFAULT_COMPILER_WINDOWS "cl"

#define REZ_DIR "bin"

#define REZ_TOOL "tool-rez"

#define REZ_CACHE_FILENAME "rez-cache.txt"

#define REZ_COMSPEC_ENV_VAR "COMSPEC"

#define REZ_VCVARS_SCRIPT "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\vcvarsall.bat"

// https://devblogs.microsoft.com/oldnewthing/20100203-00/?p=15083
#define REZ_WINDOWS_MAX_ENV_BLOCK 32760

int rez_file_status(const char *path);

int rez_load_msvc(bool reload);
