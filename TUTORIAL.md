# TUTORIAL

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
<running your tasks...>
```

## Triggering the task definition

When we execute `rez [<task names>]`, then rez compiles your task definition to a binary `bin/delegate-rez[.exe]` inside of the `.rez` cache directory. Then rez executes the delegate, triggering your task definition's `main()` entrypoint. From there, it's all your control flow.

### Task definition compilation settings

rez automatically infers a default compiler toolchain, similar to the `cmake` task runner.

For example, in Windows (COMSPEC), rez uses `vcvars.bat` to enable an MSVC cl.exe environment. rez caches a copy of the MSVC environment variable pairs in `.rez\rez-env.txt`, so that successive `rez` commands run faster.

On UNIX systems like Linux and macOS, rez applies the active `c++` or `cc` compiler, depending on the task definition language.

rez responds to common C/C++ build environment variables including `CXX`, `CC`, `CPPFLAGS`, `CXXFLAGS`, and `CFLAGS` when building the task definition.

For more information on customizing the task definition compilation step, see [include/](include).

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

rez provides a built-in flag `-c` (clear internal rez cache).

```console
$ rez -c
```
