#include <cstdlib>

#include <filesystem>
#include <functional>
#include <iostream>
#include <string_view>
#include <vector>

using std::literals::string_view_literals::operator""sv;

static int build_task() {
    const auto cmake_status = system("cmake .");

    if (cmake_status != EXIT_SUCCESS) {
        return cmake_status;
    }

    return system("cmake --build . --config Release");
}

static int run_task() {
    const auto build_status = build_task();

    if (build_status != EXIT_SUCCESS) {
        return build_status;
    }

    return system((std::filesystem::path("bin") / "athena").string().c_str());
}

int main(int argc, const char **argv) {
    const auto default_task = std::function<int()>(run_task);
    const auto tasks = std::unordered_map<std::string_view, std::function<int()>>{
        { "build"sv, build_task },
        { "run"sv, run_task }
    };

    const auto args = std::vector<std::string_view>{ argv + 1, argv + argc };

    if (args.empty()) {
        return default_task();
    }

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
