#include <cstdio>
#if defined(_MSC_VER)
#define pclose _pclose
#define popen _popen
#endif

#include <cstdlib>
#include <fstream>
#include <iostream>

#include "rez/rez.hpp"

namespace rez {
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
#if defined(_MSC_VER)
    this->compiler = DefaultCompilerWindows;
#else
    this->compiler = DefaultCompilerUnix;
#endif

    const char *key = CompilerEnvVar.c_str();
    char *compiler_override_transient = nullptr;
    auto compiler_override = std::string();

#if defined(_MSC_VER)
    auto len = size_t(0);
    errno = 0;
    _dupenv_s(&compiler_override_transient, &len, key);

    if (errno != 0) {
        std::cerr << "error querying environment variable " << key << " errno: " << errno << std::endl;
        free(compiler_override_transient);
        return -1;
    }

    if (compiler_override_transient != nullptr) {
        compiler_override = std::string(compiler_override_transient);
    }

    free(compiler_override_transient);
#else
    compiler_override_transient = getenv(key);

    if (compiler_override_transient != nullptr) {
        compiler_override = std::string(compiler_override_transient);
    }
#endif

    if (compiler_override != "") {
        this->compiler = compiler_override;
    }

    if (this->compiler == DefaultCompilerWindows && this->ApplyMSVCToolchain() < 0) {
        std::cerr << "error applying msvc toolchain" << std::endl;
        return -1;
    }

    return 0;
}

std::ostream& operator << (std::ostream &os, const Config o) {
    return os << "{ debug: " << o.debug
                << " compiler: " << o.compiler
                << " }";
}
}
