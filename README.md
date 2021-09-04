# rez: a C/C++ task runner

```text
 `_~----====-.
  \ <.+|*-*| '
 /&( ``
 \3
```

# SUMMARY

rez runs C/C++ tasks.

Where other runners depend on managing secondary DSL's, *rez* collapses the software stack. Implement your build scripts in the same language as your application.

Maximize expressiveness, minimize bloat.

# EXAMPLE

```console
$ cd example
$ rez

   ...    *    .   _  .
*  .  *     .   * (_)   *
  .      |*  ..   *   ..
   .  * \|  *  ___  . . *
*   \/   |/ \/{0,Q}     .
  _\_\   |  / /)  )* _/_ *
      \ \| /,--"-"---  ..
_-----`  |(,__,__/__/_ .
       \ ||      ..
        ||| .            *
        |||
        |||
  , -=-~' .-^- _

Must be expensive.

```

See `rez -h` for more options.

# ABOUT

rez allows developers to write build tasks in pure C/C++ code. This streamlines software builds. For example, common shell-outs to `rm` / `del`... become `std::filesystem::remove_all` in C++. As you migrate from shell scripts to native code, you are rewarded with faster, more reliable builds.

rez is *build tool agnostic*. rez simply wraps your traditional build tool of choice. For example, rez tasks can invoke direct compiler commands like `gcc`, or invoke an underlying build tool like `cmake`, `make`, `./configure`, `msbuild`, etc.

Generate and retire artifacts, like so many tears in the rain...

# LICENSE

FreeBSD

# RUNTIME REQUIREMENTS

* a C/C++ compiler

rez supports `gcc/g++`, `clang`/`clang++`, and `cl.exe`.

rez is regularly tested for cross-platform support on Linux, macOS, and Windows.

## Recommended

* [direnv](https://direnv.net/) / [posh-direnv](https://github.com/takekazuomi/posh-direnv)

# CONTRIBUTING

For information about developing rez itself, see [DEVELOPMENT.md](DEVELOPMENT.md).

# CONFIGURATION

rez works by compiling and executing your task definition source file.

| Language | Task Definition |
| -------- | --------------- |
| C        | rez.c           |
| C++      | rez.cpp         |

When the user runs `rez <task name>`..., then rez compilers your task definition and executes it.

At that point, rez hands off processing the task tree to your custom definition. rez forwards the `<task name>`... command line arguments into your task definition's `int main(int argc, char **argv)` entrypoint.

## Default task

By convention, your task definition should default to running a specific task of your choice, when no arguments are supplied. This can be a task to `build` your application, or `test` your application, for example. So that `rez` with no arguments will do something you find useful for your project. This matches common practices with `make`, for example.

## List tasks menu

In addition to `<task name>`'s, rez may forward a `-l` (list tasks) flag to your task definition. rez expects your task definition to respond to `-l` by respond by listing out the names of your tasks. This matches common practice with `mage -l`, for example.

## Install and Uninstall tasks

By convention, a project should implement a pair of `install` and `uninstall` tasks to automate the process of compiling and placing binaries into a semi-portable directory in `$PATH`. For example, have your `install` task invoke a `build` task, and then copy the resulting binary to `~/bin/<app>[.exe]`. Have your `uninstall` task delete this file.

## Clean task

By convention, a project should implement a `clean` task which removes any generated files, such as binaries, object files, or build tool junk files. The command should be idempotent, able to be run regardless of whether the files are already deleted. This helps contributors to quickly reset the project file tree back to a fresh state and reduce disk usage. This matches common practices with `make clean`, for example.

## Clean internal rez cache

rez provides a small set of flags such as `-c` (clear internal rez cache), which are not propagated to the task definition.

For example, the steps to uninstall your app, then delete junk files, then reset the rez cache, look like:

```console
$ rez uninstall; rez clean; rez -c
```

See the [example/](example) application for more detail.

# SEE ALSO

* [autotools](https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html), a preiminent GNU/Linux build system
* [batsch](https://batsh.org/), a preprocessing language that compiles to bash and MS-DOS bat files (somewhat defunct)
* [cling](https://root.cern/cling/), a C++ interpreter
* [cmake](https://cmake.org/), a preiminent cross-platform build system
* [dale](https://github.com/mcandre/dale), a D task runner
* [ejm98](http://www.ascii-art.de/ascii/mno/owl.txt) for Athena's prior artwork
* [Gradle](https://gradle.org/) / [Maven](https://maven.apache.org/) / [Ant](https://ant.apache.org/), preiminent JVM build tools
* [Grunt](https://gruntjs.com/), a Node.js task runner
* [GNU make](https://www.gnu.org/software/make/) / [BSD make](https://www.freebsd.org/cgi/man.cgi?make(1)), classic task runners
* [lake](https://luarocks.org/modules/steved/lake), a Lua task runner
* [mage](https://magefile.org/), a preiminent Go task runner
* [rake](https://ruby.github.io/rake/), a preiminent Ruby task runner
* Not to be confused with [Rezz](http://officialrezz.com/), a preiminent DJ
* [sail](https://github.com/mcandre/sail), a fast C/C++ source file identifier
* [Shake](https://shakebuild.com/), a preiminent Haskell task runner
* [stank](https://github.com/mcandre/stank), a collection of POSIX-y shell script linter tools
* [tinyrick](https://github.com/mcandre/tinyrick), a Rust task runner
* [vast](https://github.com/mcandre/vast), a POSIX sh task runner
* [WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10) / [Cygwin](https://www.cygwin.com/) / [MinGW](https://www.mingw-w64.org/) / [MSYS2](https://www.msys2.org/) / [Git Bash](https://git-scm.com/downloads) / [Strawberry Perl](https://strawberryperl.com/) provide shims for many GNU/Linux components on Windows hosts
