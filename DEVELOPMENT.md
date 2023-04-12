# BUILDTIME REQUIREMENTS

* a [C++](https://www.cplusplus.com/) compiler with full C++17 support (e.g. clang 9+, g++ 9+, MSVC 19+)
* [cmake](https://cmake.org/) 3+
* [clang-format](https://clang.llvm.org/docs/ClangFormat.html)
* [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)
* [cppcheck](https://cppcheck.sourceforge.io/)
* [Doxygen](https://www.doxygen.nl/index.html)
* [Docker](https://www.docker.com/) 20.10.12+
* [snek](https://github.com/mcandre/snek) 0.0.2 with an x86_64 host
* [GNU findutils](https://www.gnu.org/software/findutils/)
* [sail](https://github.com/mcandre/sail) 0.0.1
* [Python](https://www.python.org/) 3.11.2+
* a POSIX compliant [sh](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/sh.html) implementation (e.g. bash, ksh, zsh)
* Provision additional dev tools with `./install`

## Recommended

* [ASDF](https://asdf-vm.com/) 0.10
* [direnv](https://direnv.net/) 2

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
$ sh clean
```
