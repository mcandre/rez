#include <cstdlib>

#include <filesystem>
#include <functional>
#include <iostream>
#include <string_view>
#include <map>
#include <unordered_set>
#include <vector>

using std::literals::string_view_literals::operator""sv;

static int cmake_init() {
    return system("cmake .");
}

static int build() {
    const auto status = cmake_init();

    if (status != EXIT_SUCCESS) {
        return status;
    }

    return system("cmake --build . --config Release");
}

static int install() {
    const auto status = build();

    if (status != EXIT_SUCCESS) {
        return status;
    }

    return system("cmake --build . --target install");
}

static int uninstall() {
    const auto status = cmake_init();

    if (status != EXIT_SUCCESS) {
        return status;
    }

    return system("cmake --build . --target uninstall");
}

static int run() {
    const auto status = install();

    if (status != EXIT_SUCCESS) {
        return status;
    }

    return system("athena");
}

static int clean_msvc() {
    std::filesystem::remove_all("x64");
    std::filesystem::remove_all("x86");

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
            std::filesystem::remove_all(child_path);
        }
    }

    return EXIT_SUCCESS;
}

static int clean_cmake() {
    std::filesystem::remove_all("bin");
    std::filesystem::remove_all("Makefile");
    std::filesystem::remove_all("CMakeFiles");
    std::filesystem::remove_all("CMakeCache.txt");
    std::filesystem::remove_all("cmake_install.cmake");
    return EXIT_SUCCESS;
}

static int clean() {
    clean_msvc();
    clean_cmake();
    return EXIT_SUCCESS;
}

int main(int argc, const char **argv) {
    const auto args = std::vector<std::string_view>{ argv + 1, argv + argc };
    const auto default_task = std::function<int()>(run);

    if (args.empty()) {
        return default_task();
    }

    const auto tasks = std::map<std::string_view, std::function<int()>>{
        { "clean"sv, clean },
        { "clean_cmake"sv, clean_cmake },
        { "clean_msvc"sv, clean_msvc },
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
