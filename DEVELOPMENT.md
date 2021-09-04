# BUILDTIME REQUIREMENTS

* a C/C++ 17 compiler
* [cmake](https://cmake.org/)

## Recommended

* [GNU coreutils](https://www.gnu.org/software/coreutils/), [GNU findutils](https://www.gnu.org/software/findutils/), such as from [WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10)
* [clang-format](https://clang.llvm.org/docs/ClangFormat.html)
* [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)
* [cppcheck](https://cppcheck.sourceforge.io/)
* [cpplint](https://pypi.org/project/cpplint/)
* [Doxygen](https://www.doxygen.nl/index.html)
* [sail](https://github.com/mcandre/sail)
* [vera++](https://bitbucket.org/verateam/vera/wiki/Home)

# BUILD

```console
$ cmake .
$ cmake --build . --config Release
```

# LINT

```console
$ cmake --build . --target lint
```

# CLEAN

```console
$ ./clean-cmake.sh
```
