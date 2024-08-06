/**
 * @copyright 2021 YelloSoft
 */

#include <cstdlib>

#include <iostream>
#include <sstream>
#include <vector>

#include "rez/rez.hpp"

/**
 * @brief Usage emits operational documentation.
 *
 * @param program the invoked name of this program
 */
void Usage(const std::string_view &program) {
    std::cerr << "usage: " << program << " [OPTION] [<task> [<task> [<task>...]]]\n\n";
    std::cerr << "-l\tList available tasks\n"
              << "-c\tClean rez internal cache\n"
              << "-d\tEnable debugging information\n"
              << "-v\tShow version information\n"
              << "-h\tShow usage information\n";
}

/**
 * @brief Banner emits version information.
 */
void Banner() {
    std::cout << "rez " << rez::Version << "\n";
}

/**
 * @brief main is the entrypoint.
 *
 * @param argc argument count
 * @param argv CLI arguments
 * @returns CLI exit code
 */
int main(int argc, const char **argv) {
    const std::vector<std::string_view> args{ argv, argv + argc };

    // cppcheck-suppress knownConditionTrueFalse
    if (args.empty()) {
        std::cerr << "error: missing program name\n";
        return EXIT_FAILURE;
    }

    rez::Config config;

    size_t i{ 1 };
    for (; i < args.size(); i++) {
        const std::string_view arg{ args[i] };

        if (arg == "-c") {
            std::filesystem::remove_all(rez::CacheDir);
            return EXIT_SUCCESS;
        }

        if (arg == "-d") {
            config.debug = true;
            continue;
        }

        if (arg == "-v") {
            Banner();
            return EXIT_SUCCESS;
        }

        if (arg == "-h") {
            Usage(args[0]);
            return EXIT_SUCCESS;
        }

        break;
    }

    const std::vector<std::string_view> rest{ args.begin() + static_cast<ptrdiff_t>(i), args.end() };

    try {
        config.Load();
    } catch (const std::exception &err) {
        std::cerr << err.what() << "\n";
    }

    if (config.debug) {
        std::cerr << config << "\n";
    }

    const bool artifact_cache_miss = !std::filesystem::exists(config.artifact_file_path) ||
                                     std::filesystem::last_write_time(config.artifact_file_path) < std::filesystem::last_write_time(config.task_definition_path);

    if (artifact_cache_miss) {
        std::filesystem::create_directories(config.artifact_dir_path);
        const std::string build_command{ config.build_command };

        if (config.debug) {
            std::cerr << "running build command: " << build_command << "\n";
        }

        const int build_status{ system(build_command.c_str()) };

        if (build_status != EXIT_SUCCESS) {
            std::cerr << "error building task file: " << config.task_definition_path.string() << "\n";
            return build_status;
        }
    }

    std::stringstream ss;
    ss << config.artifact_file_path.string();

    for (const std::string_view &arg : rest) {
        ss << " ";
        ss << arg;
    }

    const std::string run_command{ ss.str() };

    if (config.debug) {
        std::cerr << "running command: " << run_command << "\n";
    }

    if (system(run_command.c_str())) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
