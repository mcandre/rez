# rez: C/C++ task runner

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
$ cd examples/athena
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

BSD-2-Clause

# RUNTIME REQUIREMENTS

* a C/C++ compiler

rez is regularly tested for cross-platform support on Linux, macOS, and Windows.

## Recommended

* [direnv](https://direnv.net/) / [posh-direnv](https://github.com/takekazuomi/posh-direnv)
* [cmake](https://cmake.org/) 3+
* [make](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/make.html) or [ninja](https://ninja-build.org/)

# CONTRIBUTING

For information about developing rez itself, see [DEVELOPMENT.md](DEVELOPMENT.md).

# SEE ALSO

* [batsch](https://batsh.org/), a preprocessing language that compiles to bash and MS-DOS bat files (somewhat defunct)
* [beltaloada](https://github.com/mcandre/beltaloada), a guide to writing build systems for (POSIX) sh
* [booty](https://github.com/mcandre/booty?tab=readme-ov-file) for JS/Node.js/altJS
* [cling](https://root.cern/cling/), a C++ interpreter
* [cmake](https://cmake.org/), a preiminent cross-platform build system
* [dale](https://github.com/mcandre/dale), a D task runner
* [ejm98](http://www.ascii-art.de/ascii/mno/owl.txt) for Athena's prior artwork
* [GNU autotools](https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html), a build system for Linux C/C++ projects
* [Gradle](https://gradle.org/), a build system for JVM projects
* [jelly](https://github.com/mcandre/jelly), a JSON task runner
* [lake](https://luarocks.org/modules/steved/lake), a Lua task runner
* [Leiningen](https://leiningen.org/) + [lein-exec](https://github.com/kumarshantanu/lein-exec), a Clojure task runner
* [lichen](https://github.com/mcandre/lichen), a sed task runner
* [Mage](https://magefile.org/), a task runner for Go projects
* [mian](https://github.com/mcandre/mian), a task runner for (Chicken) Scheme Lisp
* [npm](https://www.npmjs.com/), [Grunt](https://gruntjs.com/), Node.js task runners
* [nobuild](https://github.com/tsoding/nobuild), another freeform C/C++ task runner
* [POSIX make](https://pubs.opengroup.org/onlinepubs/009695299/utilities/make.html), a task runner standard for C/C++ and various other software projects
* [Rake](https://ruby.github.io/rake/), a task runner for Ruby projects
* [Rebar3](https://www.rebar3.org/), a build system for Erlang projects
* Not to be confused with [Rezz](http://officialrezz.com/), the DJ
* Also not to be confused with [Rez](http://preserve.mactech.com/articles/mactech/Vol.14/14.09/RezIsYourFriend/index.html), the classic Macintosh resource compiler.
* [sail](https://github.com/mcandre/sail), a fast C/C++ source file identifier
* [sbt](https://www.scala-sbt.org/index.html), a build system for Scala projects
* [Shake](https://shakebuild.com/), a preiminent Haskell task runner
* [stank](https://github.com/mcandre/stank), a collection of POSIX-y shell script linter tools
* [tinyrick](https://github.com/mcandre/tinyrick), a Rust task runner
* [WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10) / [Cygwin](https://www.cygwin.com/) / [MinGW](https://www.mingw-w64.org/) / [MSYS2](https://www.msys2.org/) / [Git Bash](https://git-scm.com/downloads) / [Strawberry Perl](https://strawberryperl.com/) provide shims for many GNU/Linux components on Windows hosts
* [yao](https://github.com/mcandre/yao), a task runner for Common LISP projects

Generate and retire artifacts, like so many tears in the rain...
