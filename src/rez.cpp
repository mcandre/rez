/**
 * @copyright 2021 YelloSoft
 */

#include <cstdio>
#if defined(_MSC_VER)
#define pclose _pclose
#define popen _popen
#endif

#include <cstdlib>
#include <cstring>

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

using std::literals::string_literals::operator""s;

#include "rez/rez.hpp"

namespace rez {
std::ostream &operator<<(std::ostream &os, const Lang &o) {
    switch (o) {
    case Lang::Cpp:
        return os << "C++";
    default:
        return os << "C";
    }
}

std::optional<std::string> GetEnvironmentVariable(const std::string &key) {
    char *transient = nullptr;

#if defined(_MSC_VER)
    auto len = size_t(0);
    errno = 0;
    _dupenv_s(&transient, &len, key.c_str());

    if (errno != 0) {
        std::cerr << "error querying environment variable " << key << " errno: " << errno << std::endl;
        free(transient);
        return std::nullopt;
    }

    if (transient != nullptr) {
        const auto s = std::string(transient);
        free(transient);
        return std::optional(s);
    }

    free(transient);
    return std::nullopt;
#else
    transient = getenv(key.c_str());

    if (transient != nullptr) {
        const auto s = std::string(transient);
        return std::optional(s);
    }
#endif

    return std::nullopt;
}

bool DetectWindowsEnvironment() {
    return GetEnvironmentVariable("COMSPEC").has_value();
}

[[nodiscard]] int Config::ApplyMSVCToolchain() const {
    std::filesystem::create_directories(CacheDir);

    if (!std::filesystem::exists(cache_file_path)) {
        std::cerr << "querying msvc toolchain..." << std::endl;

        std::stringstream ss;
        ss << R"(cmd.exe /c "")";
        ss << MSVCToolchainQueryScript;
        ss << R"(" )";
        ss << DefaultArchitectureMSVC;
        ss << R"( && set")";
        const auto query_command = ss.str();

        if (debug) {
            std::cerr << "running msvc query command: " << query_command << std::endl;
        }

        errno = 0;
        FILE *process = popen(query_command.c_str(), "r");

        if (process == nullptr) {
            std::cerr << "error launching msvc query command: " << query_command << std::endl;
            return -1;
        }

        auto cache_writer = std::ofstream();
        cache_writer.open(cache_file_path);

        // https://devblogs.microsoft.com/oldnewthing/20100203-00/?p=15083#:~:text=The%20theoretical%20maximum%20length%20of,a%20limit%20of%2032767%20characters.
        char line[32760] = { 0 };

        while (fgets(line, sizeof(line), process) != nullptr) {
            if (strchr(line, '=') != nullptr) {
                cache_writer << line;
            }
        }

        cache_writer.close();
        const auto query_status = pclose(process);

        if (query_status != EXIT_SUCCESS) {
            std::cerr << "error processing msvc query command. status: " << query_status << std::endl;
            return -1;
        }
    }

    auto cache_reader = std::ifstream();
    cache_reader.open(cache_file_path);

    std::string line;

    while (getline(cache_reader, line)) {
#if defined(_MSC_VER)
        errno = 0;
        if (_putenv(line.c_str()) != 0) {
#else
        size_t j = line.find('=');
        const auto key = line.substr(0, j - 1);
        const auto value = line.substr(j + 1, line.size());

        errno = 0;
        if (setenv(key.c_str(), value.c_str(), 1) != 0) {
#endif
            std::cerr << "error applying environment variable " << line << " errno: " << errno << std::endl;
            cache_reader.close();
            return -1;
        }
    }

    cache_reader.close();
    return 0;
}

[[nodiscard]] int Config::Load() {
    cache_dir_path = std::filesystem::path(CacheDir);
    cache_file_path = cache_dir_path / CacheFileBasename;

    windows = DetectWindowsEnvironment();

    if (std::filesystem::exists(RezDefinitionPathCpp)) {
        rez_definition_path = RezDefinitionPathCpp;
        rez_definition_lang = Lang::Cpp;
    } else if (std::filesystem::exists(RezDefinitionPathC)) {
        rez_definition_path = RezDefinitionPathC;
        rez_definition_lang = Lang::C;
    } else {
        std::cerr << "errno no such task definition file rez.cpp / rez.c" << std::endl;
        return -1;
    }

    if (windows) {
        compiler = DefaultCompilerWindows;
    } else if (rez_definition_lang == Lang::Cpp) {
        compiler = DefaultCompilerUnixCpp;
    } else {
        compiler = DefaultCompilerUnixC;
    }

    if (rez_definition_lang == Lang::Cpp) {
        const auto compiler_override = GetEnvironmentVariable("CXX"s);

        if (compiler_override.has_value()) {
            const auto compiler_override_s = *compiler_override;

            if (!compiler_override_s.empty()) {
                compiler = compiler_override_s;
            }
        }
    } else {
        const auto compiler_override = GetEnvironmentVariable("CC"s);

        if (compiler_override.has_value()) {
            const auto compiler_override_s = *compiler_override;

            if (!compiler_override_s.empty()) {
                compiler = compiler_override_s;
            }
        }
    }

    if (compiler == DefaultCompilerWindows && ApplyMSVCToolchain() < 0) {
        std::cerr << "error applying msvc toolchain" << std::endl;
        return -1;
    }

    artifact_dir_path = cache_dir_path / std::filesystem::path(ArtifactDirBasename);

    auto executable = std::filesystem::path(ArtifactBinaryUnix);

    if (windows) {
        executable += ".exe";
    }

    artifact_file_path = artifact_dir_path / executable;

    std::stringstream ss;
    ss << compiler;
    ss << " ";

    const auto flags_cpp_opt = rez::GetEnvironmentVariable("CPPFLAGS");
    std::string flags_cpp;

    if (flags_cpp_opt.has_value()) {
        const auto flags = *flags_cpp_opt;

        if (!flags.empty()) {
            flags_cpp = flags;
        }
    }

    std::string flags_cxx,
                flags_c;

    if (rez_definition_lang == Lang::Cpp) {
        const auto flags_cxx_opt = rez::GetEnvironmentVariable("CXXFLAGS");

        if (flags_cxx_opt.has_value()) {
            const auto flags = *flags_cxx_opt;

            if (!flags.empty()) {
                flags_cxx = flags;
            }
        }
    } else {
        const auto flags_c_opt = rez::GetEnvironmentVariable("CFLAGS");

        if (flags_c_opt.has_value()) {
            const auto flags = *flags_c_opt;

            if (!flags.empty()) {
                flags_c = flags;
            }
        }
    }

    const auto artifact_file_path_s = artifact_file_path.string();

    if (compiler == DefaultCompilerWindows) {
        if (!flags_cpp.empty()) {
            ss << flags_cpp;
            ss << " ";
        }

        if (rez_definition_lang == Lang::Cpp && !flags_cxx.empty()) {
            ss << flags_cxx;
            ss << " ";
        } else if (rez_definition_lang == Lang::C && !flags_c.empty()) {
            ss << flags_c;
            ss << " ";
        }

        ss << rez_definition_path;
        ss << " /link /out:";
        ss << artifact_file_path_s;
    } else {
        ss << "-o ";
        ss << artifact_file_path_s;
        ss << " ";

        if (!flags_cpp.empty()) {
            ss << flags_cpp;
            ss << " ";
        }

        if (rez_definition_lang == Lang::Cpp && !flags_cxx.empty()) {
            ss << flags_cxx;
            ss << " ";
        } else if (rez_definition_lang == Lang::C && !flags_c.empty()) {
            ss << flags_c;
            ss << " ";
        }

        ss << rez_definition_path;
    }

    build_command = ss.str();
    return 0;
}

std::ostream &operator<<(std::ostream &os, const Config &o) {
    return os << "{ debug: " << o.debug
              << ", cache_dir_path: " << o.cache_dir_path.string()
              << ", cache_file_path: " << o.cache_file_path.string()
              << ", windows: " << o.windows
              << ", rez_definition_path: " << o.rez_definition_path.string()
              << ", rez_definition_lang: " << o.rez_definition_lang
              << ", compiler: " << o.compiler
              << ", artifact_dir_path: " << o.artifact_dir_path.string()
              << ", artifact_file_path: " << o.artifact_file_path.string()
              << ", build_command: " << o.build_command
              << " }";
}
}
