#include <filesystem>

namespace rez {
static const std::string Version = "0.0.1";

static const auto RezFile = std::filesystem::path("rez.cpp");

static const auto CacheDir = std::filesystem::path(".rez");

static const auto CachePath = CacheDir / "rez-env.txt";

static const auto ArtifactDir = CacheDir / "bin";

static const auto ArtifactBinaryUnix = std::string("rez");

static const auto ExecutableExtensionWindows = std::string(".exe");

#if defined(_MSC_VER)
static const auto ArtifactBinaryPath = ArtifactDir / std::filesystem::path(ArtifactBinaryUnix + ExecutableExtensionWindows);
#else
static const auto ArtifactBinaryPath = ArtifactDir / ArtifactBinaryUnix;
#endif

static const auto DefaultCompilerUnix = std::string("c++");

static const auto DefaultCompilerWindows = std::string("cl");

static const auto CompilerEnvVar = std::string("CXX");

static const auto MSVCToolchainQueryScript = std::string("C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\vcvarsall.bat");

struct Config {
    bool debug;
    std::string compiler;

    int ApplyMSVCToolchain();

    int Load();
};

std::ostream& operator << (std::ostream &os, const Config o);
}
