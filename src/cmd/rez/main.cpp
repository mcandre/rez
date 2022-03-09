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
static void Usage(const std::string_view &program) {
    std::cerr << "usage: " << program << " [OPTION] [<task> [<task> [<task>...]]]" << std::endl
              << std::endl;

    std::cerr << "-l\tList available tasks" << std::endl
              << "-c\tClean rez internal cache" << std::endl
              << "-d\tEnable debugging information" << std::endl
              << "-v\tShow version information" << std::endl
              << "-h\tShow usage information" << std::endl;
}

/**
 * @brief Banner emits version information.
 */
static void Banner() {
    std::cout << "rez " << rez::Version << std::endl;
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

    if (args.empty()) {
        std::cerr << "error: missing program name" << std::endl;
        return EXIT_FAILURE;
    }

    rez::Config config;

    size_t i(1);
    for (; i < args.size(); i++) {
        const auto arg = args[i];

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

    const std::vector<std::string_view> rest{ args.begin() + i, args.end() };

    try {
        config.Load();
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
    }

    if (config.debug) {
        std::cerr << config << std::endl;
    }

    bool artifact_cache_miss = !std::filesystem::exists(config.artifact_file_path) ||
                               std::filesystem::last_write_time(config.artifact_file_path) < std::filesystem::last_write_time(config.task_definition_path);

    if (artifact_cache_miss) {
        std::filesystem::create_directories(config.artifact_dir_path);

        const auto build_command = config.build_command;

        if (config.debug) {
            std::cerr << "running build command: " << build_command << std::endl;
        }

        const auto build_status = system(build_command.c_str());

        if (build_status != EXIT_SUCCESS) {
            std::cerr << "error building task file: " << config.task_definition_path.string() << std::endl;
            return build_status;
        }
    }

    std::stringstream ss;
    ss << config.artifact_file_path.string();

    for (const auto &arg : rest) {
        ss << " ";
        ss << arg;
    }

    const auto run_command = ss.str();

    if (config.debug) {
        std::cerr << "running command: " << run_command << std::endl;
    }

    const auto status = system(run_command.c_str());

    if (status) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
