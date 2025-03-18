# BUILDTIME REQUIREMENTS

* [clang++](https://clang.llvm.org/) 14+ or [g++](https://gcc.gnu.org/) 12+
* [cmake](https://cmake.org/) 3.8+
* [cppcheck](https://cppcheck.sourceforge.io/) 2.10+
* [Docker](https://www.docker.com/) 20.10.12+
* [Doxygen](https://www.doxygen.nl/index.html) 9.4+
* [GNU](https://www.gnu.org/)/[BSD](https://en.wikipedia.org/wiki/Berkeley_Software_Distribution) [findutils](https://en.wikipedia.org/wiki/Find_(Unix))
* [POSIX](https://pubs.opengroup.org/onlinepubs/9799919799/) compatible [make](https://en.wikipedia.org/wiki/Make_(software))
* [Python](https://www.python.org/) 3.12.1+
* [Rust](https://www.rust-lang.org/en-US/) 1.75.0+
* [POSIX](https://pubs.opengroup.org/onlinepubs/9799919799/) compatible [sh](https://en.wikipedia.org/wiki/Unix_shell)
* [snek](https://github.com/mcandre/snek) 0.0.5
* [Snyk](https://snyk.io/)
* [POSIX](https://pubs.opengroup.org/onlinepubs/9799919799/) compatible [tar](https://en.wikipedia.org/wiki/Tar_(computing))
* Provision additional dev tools with `make -f install.mk [-j 4]`

## Recommended

* a host capable of running musl/Linux containers (e.g. a GNU/Linux, musl/Linux, macOS, or Windows host)
* [Docker First Aid Kit](https://github.com/mcandre/docker-first-aid-kit)
* Apply `DOCKER_DEFAULT_PLATFORM` = `linux/amd64` environment variable
* [ASDF](https://asdf-vm.com/) 0.10 (run `asdf reshim` after provisioning)
* [clang-format](https://clang.llvm.org/docs/ClangFormat.html)
* [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) (macOS: `brew install llvm`, then `brew link llvm --force`)
* [direnv](https://direnv.net/) 2
* [GNU](https://www.gnu.org/)/[BSD](https://en.wikipedia.org/wiki/Berkeley_Software_Distribution) [make](https://en.wikipedia.org/wiki/Make_(software))
* macOS [open](https://ss64.com/mac/open.html) or equivalent alias
* [GNU](https://www.gnu.org/)/[BSD](https://en.wikipedia.org/wiki/Berkeley_Software_Distribution) [tar](https://en.wikipedia.org/wiki/Tar_(computing))
* [tree](https://en.wikipedia.org/wiki/Tree_(command))
* a [UNIX](https://en.wikipedia.org/wiki/Unix)-like environment

# AUDIT

```console
$ make audit
```

# BUILD

```console
$ make [build]
```

# INSTALL

```console
$ make install
```

# UNINSTALL

```console
$ make uninstall
```

# LINT

```console
$ make lint
```

# DOCUMENT

```console
$ make doc
$ open html/index.html
```

# PORT

```console
$ make port
```

# CLEAN

```console
$ make clean
```
