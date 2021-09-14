# BUILDTIME REQUIREMENTS

* a C/C++ 17 compiler
* [cmake](https://cmake.org/) 3+

## Recommended

* [GNU coreutils](https://www.gnu.org/software/coreutils/), [GNU findutils](https://www.gnu.org/software/findutils/), such as from [WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10)
* [clang-format](https://clang.llvm.org/docs/ClangFormat.html)
* [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)
* [cppcheck](https://cppcheck.sourceforge.io/)
* [cpplint](https://pypi.org/project/cpplint/)
* [Doxygen](https://www.doxygen.nl/index.html)
* [sail](https://github.com/mcandre/sail)
* [snek](https://github.com/mcandre/snek) 0.0.1
* [vera++](https://bitbucket.org/verateam/vera/wiki/Home)

# BUILD

```console
$ cmake .
$ cmake --build . --config Release
```

# INSTALL

```console
$ cmake --build . --target install
```

# UNINSTALL

```console
$ cmake --build . --target uninstall
```

# LINT

```console
$ cmake --build . --target lint
```

# DOCUMENT

```console
# cmake --build . --target doc
$ karp html/index.html
```

# PORT

```console
$ snek
```

# CLEAN

```console
$ ./clean.sh
```
