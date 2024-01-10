# BUILDTIME REQUIREMENTS

* a [C++](https://www.cplusplus.com/) compiler with full C++17 support (e.g. clang 9+, g++ 9+, MSVC 19+)
* [cmake](https://cmake.org/) 3+
* [clang-format](https://clang.llvm.org/docs/ClangFormat.html)
* [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)
* [cppcheck](https://cppcheck.sourceforge.io/)
* [Doxygen](https://www.doxygen.nl/index.html)
* [Docker](https://www.docker.com/) 20.10.12+
* GNU or BSD [findutils](https://en.wikipedia.org/wiki/Find_(Unix))
* POSIX compatible [make](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/make.html)
* [Python](https://www.python.org/) 3.12.1+
* [Rust](https://www.rust-lang.org/en-US/) 1.68.2+
* [sail](https://github.com/mcandre/sail) 0.0.2
* POSIX compatible [sh](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/sh.html)
* [snek](https://github.com/mcandre/snek) 0.0.2 with an x86_64 host
* Provision additional dev tools with `make -f install.mk`

## Recommended

* [ASDF](https://asdf-vm.com/) 0.10 (run `asdf reshim` after provisioning)
* [direnv](https://direnv.net/) 2
* macOS [open](https://ss64.com/mac/open.html) or equivalent alias

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
$ open html/index.html
```

# PORT

```console
$ snek
```

# CLEAN

```console
$ sh clean
```
