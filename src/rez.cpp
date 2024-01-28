/**
 * @copyright 2021 YelloSoft
 */

#include <cstdio>
#if defined(_WIN32)
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

std::filesystem::path ApplyBinaryExtension(const std::filesystem::path &basename, bool windows) {
    std::filesystem::path path(basename);

    if (windows) {
        path += ".exe";
    }

    return path;
}

std::optional<std::string> GetEnvironmentVariable(const std::string &key) {
    char *transient{ nullptr };

#if defined(_WIN32) && !defined(__MINGW32__)
    size_t len{ 0 };
    errno = 0;
    _dupenv_s(&transient, &len, key.c_str());

    if (errno != 0) {
        std::cerr << "error querying environment variable " << key << " errno: " << errno << "\n";
        free(transient);
        return std::nullopt;
    }

    if (transient != nullptr) {
        const std::string s{ transient };
        free(transient);
        return s;
    }

    free(transient);
#else
    transient = getenv(key.c_str());

    if (transient != nullptr) {
        std::string s{ transient };
        return s;
    }
#endif

    return std::nullopt;
}

bool DetectWindowsEnvironment() {
    return GetEnvironmentVariable("COMSPEC").has_value();
}

void Config::ApplyMSVCToolchain() const {
    std::filesystem::create_directories(CacheDir);

    std::fstream cache{};
    cache.open(cache_file_path);

    std::error_code ec;
    const std::uintmax_t cache_size{ std::filesystem::file_size(cache_file_path, ec) };

    if (cache_size == static_cast<std::uintmax_t>(-1) || cache_size == 0) {
        if (debug) {
            std::cerr << "querying msvc toolchain...\n";
        }

        std::string query_path(DefaultMSVCToolchainQueryScript);
        const std::optional<std::string> query_path_override{ GetEnvironmentVariable("REZ_TOOLCHAIN_QUERY_PATH") };

        if (query_path_override.has_value()) {
            query_path = *query_path_override;
        }

        std::string arch(ArchitectureMsvcAmd64);
        const std::optional<std::string> arch_override{ GetEnvironmentVariable("REZ_ARCH") };

        if (arch_override.has_value()) {
            arch = *arch_override;
        }

        std::stringstream ss;
        ss << R"(cmd.exe /c "")";
        ss << query_path;
        ss << R"(" )";
        ss << arch;
        ss << R"( && set")";
        const std::string query_command{ ss.str() };

        if (debug) {
            std::cerr << "running msvc query command: " << query_command << "\n";
        }

        errno = 0;
        FILE *process{ popen(query_command.c_str(), "r") };

        if (process == nullptr) {
            throw std::runtime_error{ "error launching msvc query command: "s + query_command };
        }

        // https://devblogs.microsoft.com/oldnewthing/20100203-00/?p=15083#:~:text=The%20theoretical%20maximum%20length%20of,a%20limit%20of%2032767%20characters.
        char line[32760]{ 0 };

        while (fgets(line, sizeof(line), process) != nullptr) {
            if (strchr(line, '=') != nullptr) {
                cache << line;
            }
        }

        const int query_status{ pclose(process) };

        if (query_status != EXIT_SUCCESS) {
            cache.close();

            std::stringstream err;
            err << "error running query command: "
                << query_command
                << " status: " << query_status;
            throw std::runtime_error{ err.str() };
        }

        cache.seekg(0);
    }

    std::string line;

    while (getline(cache, line)) {
#if defined(_WIN32)
        errno = 0;
        if (_putenv(line.c_str()) != 0) {
#else
        const size_t j{ line.find('=') };
        const std::string key{ line.substr(0, j - 1) };
        const std::string value{ line.substr(j + 1, line.size()) };

        errno = 0;
        if (setenv(key.c_str(), value.c_str(), 1) != 0) {
#endif
            cache.close();

            std::stringstream err;
            err << "error applying environment variable key=value pair: "
                << line
                << " errno: "
                << errno;
            throw std::runtime_error{ err.str() };
        }
    }

    cache.close();
}

void Config::Load() {
    windows = DetectWindowsEnvironment();

    if (!std::filesystem::exists(TaskDefinitionCpp)) {
        if (std::filesystem::exists(TaskDefinitionC)) {
            task_definition_path = TaskDefinitionC;
            task_definition_lang = Lang::C;
        } else {
            throw std::runtime_error("error locating a task definition file rez.{cpp,c}");
        }
    }

    if (windows) {
        compiler = DefaultCompilerWindows;
    } else if (task_definition_lang == Lang::C) {
        compiler = DefaultCompilerUnixC;
    }

    if (task_definition_lang == Lang::Cpp) {
        const std::optional<std::string> compiler_override{ GetEnvironmentVariable("CXX"s) };

        if (compiler_override.has_value()) {
            const std::string &compiler_override_s = *compiler_override;

            if (!compiler_override_s.empty()) {
                compiler = compiler_override_s;
            }
        }
    } else {
        const std::optional<std::string> compiler_override{ GetEnvironmentVariable("CC"s) };

        if (compiler_override.has_value()) {
            const std::string &compiler_override_s = *compiler_override;

            if (!compiler_override_s.empty()) {
                compiler = compiler_override_s;
            }
        }
    }

    if (compiler == DefaultCompilerWindows) {
        ApplyMSVCToolchain();
    }

    artifact_file_path = ApplyBinaryExtension(
        artifact_dir_path / ArtifactFileBasenameUnix,
        windows);

    std::stringstream ss;
    ss << compiler;
    ss << " ";

    const std::optional<std::string> flags_cpp_opt{ rez::GetEnvironmentVariable("CPPFLAGS") };
    std::string flags_cpp;

    if (flags_cpp_opt.has_value()) {
        const std::string &flags = *flags_cpp_opt;

        if (!flags.empty()) {
            flags_cpp = flags;
        }
    }

    std::string flags_cxx,
        flags_c;

    if (task_definition_lang == Lang::Cpp) {
        const std::optional<std::string> flags_cxx_opt{ rez::GetEnvironmentVariable("CXXFLAGS") };

        if (flags_cxx_opt.has_value()) {
            const std::string &flags = *flags_cxx_opt;

            if (!flags.empty()) {
                flags_cxx = flags;
            }
        }
    } else {
        const std::optional<std::string> flags_c_opt{ rez::GetEnvironmentVariable("CFLAGS") };

        if (flags_c_opt.has_value()) {
            const std::string &flags = *flags_c_opt;

            if (!flags.empty()) {
                flags_c = flags;
            }
        }
    }

    const std::string artifact_file_path_s{ artifact_file_path.string() };

    if (compiler == DefaultCompilerWindows) {
        if (!flags_cpp.empty()) {
            ss << flags_cpp;
            ss << " ";
        }

        if (task_definition_lang == Lang::Cpp && !flags_cxx.empty()) {
            ss << flags_cxx;
            ss << " ";
        } else if (task_definition_lang == Lang::C && !flags_c.empty()) {
            ss << flags_c;
            ss << " ";
        }

        ss << task_definition_path;
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

        if (task_definition_lang == Lang::Cpp && !flags_cxx.empty()) {
            ss << flags_cxx;
            ss << " ";
        } else if (task_definition_lang == Lang::C && !flags_c.empty()) {
            ss << flags_c;
            ss << " ";
        }

        ss << task_definition_path;
    }

    build_command = ss.str();
}

std::ostream &operator<<(std::ostream &os, const Config &o) {
    return os << "{ cache_file_path: " << o.cache_file_path
              << ", debug: " << o.debug
              << ", windows: " << o.windows
              << ", task_definition_path: " << o.task_definition_path.string()
              << ", task_definition_lang: " << o.task_definition_lang
              << ", compiler: " << o.compiler
              << ", artifact_dir_path: " << o.artifact_dir_path.string()
              << ", artifact_file_path: " << o.artifact_file_path.string()
              << ", build_command: " << o.build_command
              << " }";
}
}
