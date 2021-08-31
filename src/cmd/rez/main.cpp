#include <cstdlib>
#include <iostream>
#include <vector>

#include "rez/rez.hpp"

void Usage(const std::string program) {
    std::cerr << "usage: " << program << " [OPTION] [<task> [<task> [<task>...]]]" << std::endl << std::endl;

    std::cerr << "-l\tList available tasks" << std::endl
              << "-c\tClean rez internal cache" << std::endl
              << "-d\tEnable debugging information" << std::endl
              << "-v\tShow version information" << std::endl
              << "-h\tShow usage information" << std::endl;
}

static void Banner() {
    std::cout << "rez " << rez::Version << std::endl;
}

int main(int argc, const char **argv) {
    rez::Config config;

    auto args = std::vector<std::string_view>{ argv, argv + argc };

    size_t i;
    for (i = size_t(1); i < args.size(); i++) {
        const auto arg = args[i];

        if (arg == "-c") {
            std::cerr << "unimplemented" << std::endl;
            return EXIT_FAILURE;
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
            Usage(argv[0]);
            return EXIT_SUCCESS;
        }

        break;
    }

    // const auto rest = std::vector<std::string_view>{ args.begin() + i, args.end() };

    if (config.Load() < 0) {
        std::cerr << "error loading config" << std::endl;
        return EXIT_FAILURE;
    }

    if (config.debug) {
        std::cerr << config << std::endl;
    }

    if (!std::filesystem::exists(rez::ArtifactBinaryPath)) {
        std::stringstream ss;
        ss << config.compiler;
        ss << " ";

        const auto artifact_binary_path_s = rez::ArtifactBinaryPath.string();

        if (config.compiler == rez::DefaultCompilerWindows) {
            ss << rez::RezFile.string();
            ss << " /link /out ";
            ss << artifact_binary_path_s;
        } else {
            ss << "-o ";
            ss << artifact_binary_path_s;
            ss << " ";
            ss << rez::RezFile.string();
        }

        const auto command = ss.str();

        std::cerr << "running build command: " << command << std::endl;

        // ...
    }

    // ...

    return EXIT_SUCCESS;
}
