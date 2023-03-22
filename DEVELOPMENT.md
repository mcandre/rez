# BUILDTIME REQUIREMENTS

* [Go](https://go.dev/) 1.19+ with `go install github.com/mcandre/accio/cmd/accio@v0.0.3` and `accio -install`
* a [C++](https://www.cplusplus.com/) compiler with full C++17 support (e.g. clang 9+, g++ 9+, MSVC 19+)
* [cmake](https://cmake.org/) 3+
* [Python](https://www.python.org/) 3.11.2+ with `pip[3] install --upgrade pip setuptools` and `pip[3] install -r requirements-dev.txt`

## Recommended

* [ASDF](https://asdf-vm.com/) 0.10
* [direnv](https://direnv.net/) 2
* [GNU coreutils](https://www.gnu.org/software/coreutils/), [GNU findutils](https://www.gnu.org/software/findutils/), such as from [WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10)
* [clang-format](https://clang.llvm.org/docs/ClangFormat.html)
* [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)
* [cppcheck](https://cppcheck.sourceforge.io/)
* [Doxygen](https://www.doxygen.nl/index.html)
* [sail](https://github.com/mcandre/sail) 0.0.1
* [snek](https://github.com/mcandre/snek) 0.0.2 with an x86_64 host

# AUDIT

```console
$ cmake .
$ cmake --build . --target audit
```

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
