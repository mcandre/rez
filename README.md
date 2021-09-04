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

When we execute `rez <task names>`..., then rez compiles your task definition to a binary `.rez/bin/delegate-rez[.exe]`. The task definition compilation step is itself customizable.

rez automatically discovers the available compiler toolchain. Similar to the `cmake` task runner.

For example, in Windows (COMSPEC), rez uses `vcvars.bat` to enable an MSVC cl.exe environment. On UNIX systems like Linux and macOS, rez applies the active `c++` or `cc` compiler, depending on the task definition language.

rez responds to common C/C++ build environment variables including `CXX`, `CC`, `CPPFLAGS`, `CXXFLAGS`, and `CFLAGS` when building the task definition. See [include/](include) for more detail.

Your task definition program has full control over the task tree. You can write your own specialized task tree processing algorithm, or simply chain tasks together with ordinary function calls.

(Includes elided for brevity. For more detail, the example app contains fully qualified include directives.)

```c++
static int build() {
    return system("cmake --build . --config Release");
}

static int run() {
    // This run task depends on the build task.
    const auto status = build();

    if (status != EXIT_SUCCESS) {
        return status;
    }

    // Locate application binary in an operating system portable manner.
    return system((std::filesystem::path("bin") / "athena").string().c_str());
}
```

After rez builds your task definition, rez executes it. At this point, your task definition program receives a copy of the `<task names>` flags into its main entrypoint. Your implementation should call the particular tasks, depending on which ones are passed in on the command line.

```c++
int main(int argc, const char **argv) {
    const auto tasks = std::map<std::string_view, std::function<int()>>{
        { "build"sv, build },
        { "run"sv, run }
    };

    for (const auto &arg : args) {
        try {
            const auto f = tasks.at(arg);
            const auto status = f();

            if (status != EXIT_SUCCESS) {
                return status;
            }
        } catch (std::out_of_range &e) {
            std::cerr << "no such task: " << arg << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
```

## Default task

By convention, your task definition should default to running a specific task of your choice, when no arguments are supplied. Similar to configurations for the `npm` task runer.

This can be a task to `build` your application, or `test` your application, for example. Whichever custom task you find to be useful for your project.

```c++
int main(int argc, const char **argv) {
    const auto args = std::vector<std::string_view>{ argv + 1, argv + argc };
    const auto default_task = std::function<int()>(run);

    if (args.empty()) {
        return default_task();
    }
}
```

## List tasks menu

In addition to `<task name>`'s, rez may forward a `-l` (list tasks) flag to your task definition. Similar to configurations for the `mage` task runner.

rez expects your task definition to respond to `-l` by listing out the names of your tasks.

```c++
int main(int argc, const char **argv) {
    const auto args = std::vector<std::string_view>{ argv + 1, argv + argc };

    const auto tasks = std::map<std::string_view, std::function<int()>>{
        { "build"sv, build },
        { "run"sv, run }
    };

    if (args.front() == "-l") {
        for (const auto &[name, _] : tasks) {
            std::cout << name << std::endl;
        }

        return EXIT_SUCCESS;
    }
}
```

Trace:

```console
$ rez -l
build
run
```

## Clean, Install, and Uninstall tasks

By convention, a project should implement a pair of `install` and `uninstall` tasks to automate the process of compiling and placing binaries into a semi-portable directory in `$PATH`. For example, have your `install` task invoke a `build` task, and then copy the resulting binary to `~/bin/<app>[.exe]`. Have your `uninstall` task delete this file.

By convention, a project should implement a `clean` task which removes any generated files, such as binaries, object files, or build tool junk files. The command should be idempotent, able to be run regardless of whether the files are already deleted. This helps contributors to quickly reset the project file tree back to a fresh state and reduce disk usage. Similar to the `make` task runner.

See the [example/](example) Athena owl application for more detail.

## Clean internal rez cache

rez provides a small set of flags such as `-c` (clear internal rez cache), which are not propagated to the task definition.

For example, the steps to uninstall your app, then delete junk files, then reset the rez cache, look like:

```console
$ rez uninstall; rez clean; rez -c
```

See `rez -h` for more options.

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
