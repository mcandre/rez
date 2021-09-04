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

See `rez -h` for more options.

# ABOUT

rez allows developers to write build tasks in pure C/C++ code. This streamlines software builds. For example, common shell-outs to `rm` / `del`... become `std::filesystem::remove_all` in C++. As you migrate from shell scripts to native code, you are rewarded with faster, more reliable builds.

rez is *build tool agnostic*. rez simply wraps your traditional build tool of choice. For example, rez tasks can invoke direct compiler commands like `gcc`, or invoke an underlying build tool like `cmake`, `make`, `./configure`, `msbuild`, etc.

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

# USAGE

rez works by compiling and executing your task definition source file.

| Language | Task Definition |
| -------- | --------------- |
| C        | rez.c           |
| C++      | rez.cpp         |

Edit a task definition file at the top level of your project directory.

Example:

```console
$ cd <your project>
$ vi rez.cpp

$ rez
```

See `rez -d` to enable debugging output.

## Triggering the task definition

When we execute `rez [<task names>]`, then rez compiles your task definition to a binary `bin/delegate-rez[.exe]` inside of the `.rez` cache directory. Then rez executes the delegate, triggering your task definition's `main()` entrypoint. From there, it's all your control flow.

### Task definition build settings

rez queries standard locations to automatically infer the compiler toolchain, similar to the `cmake` task runner.

For example, in Windows (COMSPEC), rez uses `vcvars.bat` to enable an MSVC cl.exe environment. rez caches a copy of the MSVC environment variable pairs in `.rez\rez-env.txt`, so that successive `rez` commands run faster.

On UNIX systems like Linux and macOS, rez applies the active `c++` or `cc` compiler, depending on the task definition language.

rez responds to common C/C++ build environment variables including `CXX`, `CC`, `CPPFLAGS`, `CXXFLAGS`, and `CFLAGS` when building the task definition. See [include/](include) for more detail.

## Defining custom tasks

Your task definition program has full control over the task tree.

In this way, rez is designed as a free-form task runner. We offer an optional API `rez/rez.hpp` to query build metadata; It is not necessary to include this. You can use Boost and other popular libraries. The possiblities are endless.

You can write your own specialized task tree processing algorithm, or simply chain tasks together with ordinary function calls.

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

After rez builds your task definition, rez executes it. Now, your task definition program receives a copy of the `<task names>` flags into its main entrypoint. Your implementation should iterate through the command line arguments and execute the associated tasks.

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

By convention, a task definition should feature a default task, which executes when no arguments are supplied. Similar to configuration for the `npm` task runer.

For example, this can be a task to `build` or `test` your application. Whichever custom task you find to be most relevant for your project.

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

By convention, a project should implement a `clean` task which removes any generated files, such as binaries, object files, or build tool junk files. Similar to the `make` task runner.

Clean commands should be idempotent, succeeding regardless of whether the files are already deleted. This helps contributors to quickly reset the project file tree back to a fresh state and reduce disk usage.

*WARNING: Always backup your project, and print out the paths to be removed, before actually implementing file removal.*

```c++
static int clean_msvc() {
    // std::filesystem::remove_all("x64");
    // std::filesystem::remove_all("x86");

    const auto junk_extensions = std::unordered_set<std::string>{
        ".dir",
        ".filters",
        ".obj",
        ".sln",
        ".vcxproj"
    };

    for (const auto &child : std::filesystem::directory_iterator(std::filesystem::current_path())) {
        const auto child_path = child.path();

        if (junk_extensions.find(child_path.extension().string()) != junk_extensions.end()) {
            std::cout << "Testing path match. This path would be removed: " << child_path << std::endl;
            // std::filesystem::remove_all(child_path);
        }
    }

    return EXIT_SUCCESS;
}

static int clean_cmake() {
    // std::filesystem::remove_all("bin");
    // std::filesystem::remove_all("Makefile");
    // std::filesystem::remove_all("CMakeFiles");
    // std::filesystem::remove_all("CMakeCache.txt");
    // std::filesystem::remove_all("cmake_install.cmake");
    return EXIT_SUCCESS;
}

static int clean() {
    clean_msvc();
    clean_cmake();
    return EXIT_SUCCESS;
}
```

See the [example/](example) Athena owl application for more detail.

## Clean internal rez cache

rez provides a small set of flags such as `-c` (clear internal rez cache), which are not propagated to the task definition.

For example, we can run the tasks to uninstall the development edition of your app; then delete junk build files; then reset the internal rez cache, like:

```console
$ rez uninstall

$ rez clean

$ rez -c
```

Generate and retire artifacts, like so many tears in the rain...

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
