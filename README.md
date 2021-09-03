# rez: a C/C++ task runner

Generate and retire artifacts, like so many tears in the rain...

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

# LICENSE

FreeBSD

# SUMMARY

rez runs C/C++ tasks.

Where other runners depend on managing secondary DSL's, *rez* collapses the software stack. Application and build scripts: One programming language.

* Minimize environment bloat.
* Maximize expressiveness.

# RUNTIME REQUIREMENTS

```text
 `_~----====-.
  \ <.+|*-*| '
 /&( ``
 \3
```

* a C/C++ 17 compiler

## Recommended

* [direnv](https://direnv.net/) / [posh-direnv](https://github.com/takekazuomi/posh-direnv)

# ABOUT

rez observes a few particular pain points of software development, including hidden bugs and vendor locking.

In application code, these pain points are already managed with a variety of classic software techniques. For example, the C/C++ (native) programming languages illuminate many potential kinds of bugs and eliminate them at compile time. In terms of vendor locking, native documentation clarifies platform support. Native languages feature more predictable error handling semantics than shell scripts. And of course, compiled code runs faster than interpreted code.

We want to bring this same rigor in applications, to build systems. For example, common shell-outs to `rm` / `del`... become `std::filesystem::remove_all` in C++.

rez simply wraps your desired build tasks; These can be underlying cmake/make/autotools/etc. commands. rez offers some zero conf defaults, but in general promotes flexibility with the rest of the C/C++ ecosystem.

# SEE ALSO

* [autotools](https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html), a preiminent GNU/Linux build system
* [batsch](https://batsh.org/), a preprocessing language that compiles to bash and MS-DOS bat files (somewhat defunct)
* [cling](https://root.cern/cling/), a C++ interpreter
* [cmake](https://cmake.org/), a preiminent cross-platform build system
* [dale](https://github.com/mcandre/dale), a D task runner
* [Gradle](https://gradle.org/) / [Maven](https://maven.apache.org/) / [Ant](https://ant.apache.org/), preiminent JVM build tools
* [Grunt](https://gruntjs.com/), a Node.js task runner
* [GNU make](https://www.gnu.org/software/make/) / [BSD make](https://www.freebsd.org/cgi/man.cgi?make(1)), classic task runners
* [lake](https://luarocks.org/modules/steved/lake), a Lua task runner
* [mage](https://magefile.org/), a preiminent Go task runner
* [rake](https://ruby.github.io/rake/), a preiminent Ruby task runner
* [Shake](https://shakebuild.com/), a preiminent Haskell task runner
* [stank](https://github.com/mcandre/stank), a collection of POSIX-y shell script linter tools
* [tinyrick](https://github.com/mcandre/tinyrick), a Rust task runner
* [vast](https://github.com/mcandre/vast), a POSIX sh task runner
* [WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10) / [Cygwin](https://www.cygwin.com/) / [MinGW](https://www.mingw-w64.org/) / [MSYS2](https://www.msys2.org/) / [Git Bash](https://git-scm.com/downloads) / [Strawberry Perl](https://strawberryperl.com/) provide shims for many GNU/Linux components on Windows hosts
