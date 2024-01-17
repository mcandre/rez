#include <cstdlib>

#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <string_view>
#include <unordered_set>
#include <vector>

using std::literals::string_view_literals::operator""sv;

static int cmake_init() {
    return system("cmake -B build .");
}

static int build() {
    const int status{ cmake_init() };

    if (status) {
        return status;
    }

    return system("cmake --build build --config Release");
}

static int install() {
    const int status{ build() };

    if (status) {
        return status;
    }

    return system("cmake --build build --target install");
}

static int uninstall() {
    const int status{ cmake_init() };

    if (status) {
        return status;
    }

    return system("cmake --build build --target uninstall");
}

static int run() {
    const int status{ install() };

    if (status) {
        return status;
    }

    return system("athena");
}

static int clean_bin() {
    std::filesystem::remove_all("bin");
    return EXIT_SUCCESS;
}

static int clean_cmake() {
    std::filesystem::remove_all("build");
    return EXIT_SUCCESS;
}

static int clean() {
    clean_bin();
    clean_cmake();
    return EXIT_SUCCESS;
}

int main(int argc, const char **argv) {
    const std::vector<std::string_view> args{ argv + 1, argv + argc };
    const std::function<int()> default_task{ run };

    if (args.empty()) {
        if (default_task()) {
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    const std::map<std::string_view, std::function<int()>> tasks{
        { "clean"sv, clean },
        { "clean_bin"sv, clean_bin },
        { "clean_cmake"sv, clean_cmake },
        { "cmake_init"sv, cmake_init },
        { "build"sv, build },
        { "install"sv, install },
        { "run"sv, run },
        { "uninstall"sv, uninstall }
    };

    if (args.front() == "-l") {
        for (const auto &[name, _] : tasks) {
            std::cout << name << std::endl;
        }

        return EXIT_SUCCESS;
    }

    for (const std::string_view &arg : args) {
        try {
            const std::function<int()> f{ tasks.at(arg) };

            if (f()) {
                return EXIT_FAILURE;
            }
        } catch (std::out_of_range &e) {
            std::cerr << "no such task: " << arg << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
