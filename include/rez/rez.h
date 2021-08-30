#include <stdbool.h>

#define REZ_VERSION "0.0.1"

#define REZ_CACHE_FILENAME "rez-cache.txt"

#define REZ_DEFAULT_UNIX_COMPILER "c++"

#define REZ_DEFAULT_WINDOWS_COMPILER "cl"

#define REZ_COMSPEC_ENV_VAR "COMSPEC"

#define REZ_COMPILER_CPP_ENV_VAR "CXX"

#define REZ_COMPILER_C_ENV_VAR "CC"

#define REZ_VCVARS_SCRIPT "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\vcvarsall.bat"

// https://devblogs.microsoft.com/oldnewthing/20100203-00/?p=15083
#define REZ_WINDOWS_MAX_ENV_BLOCK 32760

int rez_load_msvc(bool reload);
