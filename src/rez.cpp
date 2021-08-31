#include <cstdio>
#if defined(_MSC_VER)
#define pclose _pclose
#define popen _popen
#endif

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>

#include "rez/rez.hpp"

namespace rez {
std::optional<std::string> GetEnvironmentVariable(const std::string key) {
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

int Config::ApplyMSVCToolchain() {
    std::filesystem::create_directories(CacheDir);

    if (!std::filesystem::exists(CachePath)) {
        std::cerr << "querying msvc toolchain..." << std::endl;

        const auto command = std::string("powershell.exe -Command \"cmd /c \"\""
                                + MSVCToolchainQueryScript
                                + "\"\"\" & set");

        errno = 0;
        FILE *process = popen(command.c_str(), "r");

        if (process == nullptr) {
            std::cerr << "error launching command: " << command << std::endl;
            return -1;
        }

        auto cache_writer = std::ofstream();
        cache_writer.open(CachePath);

        char line[1024] = { 0 };

        while (fgets(line, sizeof(line), process) != nullptr) {
            cache_writer << line;
        }

        cache_writer.close();

        if (pclose(process) != EXIT_SUCCESS) {
            std::cerr << "error closing process handle: " << CachePath << std::endl;
            return -1;
        }
    }

    auto cache_reader = std::ifstream();
    cache_reader.open(CachePath);

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

int Config::Load() {
    this->windows = DetectWindowsEnvironment();

    if (this->windows) {
        this->compiler = DefaultCompilerWindows;
    } else {
        this->compiler = DefaultCompilerUnix;
    }

    const auto compiler_override = GetEnvironmentVariable(CompilerEnvVar);

    if (compiler_override.has_value()) {
        const auto compiler_override_s = *compiler_override;

        if (compiler_override_s != "") {
            this->compiler = compiler_override_s;
        }
    }

    if (this->compiler == DefaultCompilerWindows && this->ApplyMSVCToolchain() < 0) {
        std::cerr << "error applying msvc toolchain" << std::endl;
        return -1;
    }

    auto executable = std::filesystem::path(ArtifactBinaryUnix);

    if (this->windows) {
        executable += ExecutableExtensionWindows;
    }

    this->artifact_path = ArtifactDir / executable;
    return 0;
}

std::ostream& operator << (std::ostream &os, const Config o) {
    return os << "{ debug: " << o.debug
                << ", windows: " << o.windows
                << ", compiler: " << o.compiler
                << ", artifact_path: " << o.artifact_path.string()
                << " }";
}
}
