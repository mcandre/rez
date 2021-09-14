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

# DEMO

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

See the full [TUTORIAL.md](TUTORIAL.md).

# ABOUT

rez allows developers to write build tasks in pure C/C++ code. This streamlines software builds. For example, common shell-outs to `rm` / `del`... become `std::filesystem::remove_all` in C++. As you migrate from shell scripts to native code, you are rewarded with faster, more reliable builds.

rez is *build tool agnostic*. rez simply wraps your traditional build tool of choice. For example, rez tasks can invoke direct compiler commands like `gcc`, or invoke an underlying build tool like `cmake`, `make`, `./configure`, `msbuild`, etc.

# DOCUMENTATION

https://mcandre.github.io/rez/

# DOWNLOAD

https://github.com/mcandre/rez/releases

# LICENSE

FreeBSD

# RUNTIME REQUIREMENTS

* a C/C++ compiler

rez is regularly tested for cross-platform support on Linux, macOS, and Windows.

## Recommended

* [direnv](https://direnv.net/) / [posh-direnv](https://github.com/takekazuomi/posh-direnv)

# CONTRIBUTING

For information about developing rez itself, see [DEVELOPMENT.md](DEVELOPMENT.md).

# SEE ALSO

* [autotools](https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html), a preiminent GNU/Linux build system
* [batsch](https://batsh.org/), a preprocessing language that compiles to bash and MS-DOS bat files (somewhat defunct)
* [cling](https://root.cern/cling/), a C++ interpreter
* [cmake](https://cmake.org/), a preiminent cross-platform build system
* [dale](https://github.com/mcandre/dale), a D task runner
* [ejm98](http://www.ascii-art.de/ascii/mno/owl.txt) for Athena's prior artwork
* [Gradle](https://gradle.org/) / [Maven](https://maven.apache.org/) / [Ant](https://ant.apache.org/), preiminent JVM build tools
* [GNU make](https://www.gnu.org/software/make/) / [BSD make](https://www.freebsd.org/cgi/man.cgi?make(1)), classic task runners
* [lake](https://luarocks.org/modules/steved/lake), a Lua task runner
* [mage](https://magefile.org/), a preiminent Go task runner
* [npm](https://www.npmjs.com/), [Grunt](https://gruntjs.com/), Node.js task runners
* [rake](https://ruby.github.io/rake/), a preiminent Ruby task runner
* Not to be confused with [Rezz](http://officialrezz.com/), a preiminent DJ
* [sail](https://github.com/mcandre/sail), a fast C/C++ source file identifier
* [Shake](https://shakebuild.com/), a preiminent Haskell task runner
* [stank](https://github.com/mcandre/stank), a collection of POSIX-y shell script linter tools
* [tinyrick](https://github.com/mcandre/tinyrick), a Rust task runner
* [vast](https://github.com/mcandre/vast), a POSIX sh task runner
* [WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10) / [Cygwin](https://www.cygwin.com/) / [MinGW](https://www.mingw-w64.org/) / [MSYS2](https://www.msys2.org/) / [Git Bash](https://git-scm.com/downloads) / [Strawberry Perl](https://strawberryperl.com/) provide shims for many GNU/Linux components on Windows hosts

Generate and retire artifacts, like so many tears in the rain...
