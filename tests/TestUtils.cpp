#include "TestUtils.h"

#include <cstring>
#include <fstream>
#include <iostream>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace dorado::tests {

std::filesystem::path get_data_dir(const std::string& sub_dir) {
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    char raw_path[PATH_MAX]{};

    CFURLRef root_url = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    if (!CFURLGetFileSystemRepresentation(
                root_url, true, reinterpret_cast<unsigned char*>(raw_path), sizeof(raw_path))) {
        std::cerr << "Failed to resolve bundle path." << std::endl;
        exit(1);
    }
    CFRelease(root_url);

    const auto root_path = std::filesystem::path(raw_path) / "data";
#else
    const std::filesystem::path root_path("./tests/data/");
#endif

    const auto data_path = root_path / sub_dir;
    if (!std::filesystem::exists(data_path)) {
        std::cerr << "Datapath " << data_path
                  << " does not exist, exiting.\n"
                     "Unit tests must be run from the root directory of the dorado checkout"
                  << std::endl;
        exit(1);
    }
    return data_path;
}

TempDir make_temp_dir(const std::string& prefix) {
#ifdef _WIN32
    std::filesystem::path path;
    while (true) {
        char temp[L_tmpnam];
        const char* name = std::tmpnam(temp);
        auto filename = prefix + std::string("_") + std::string(name);
        if (std::filesystem::create_directories(filename)) {
            path = std::filesystem::canonical(filename);
            break;
        }
    }
#else
    // macOS (rightfully) complains about tmpnam() usage, so make use of mkdtemp() on platforms that support it
    std::string temp = (std::filesystem::temp_directory_path() / (prefix + "_XXXXXXXXXX")).string();
    const char* name = mkdtemp(temp.data());
    auto path = std::filesystem::canonical(name);
#endif
    return TempDir(std::move(path));
}

std::string ReadFileIntoString(const std::filesystem::path& path) {
    const auto num_bytes = std::filesystem::file_size(path);
    std::string content;
    content.resize(num_bytes);
    std::ifstream in_file(path.c_str(), std::ios::in | std::ios::binary);
    in_file.read(content.data(), content.size());
    return content;
}

std::vector<uint8_t> ReadFileIntoVector(const std::filesystem::path& path) {
    const std::string str = ReadFileIntoString(path);
    std::vector<uint8_t> vec;
    vec.resize(str.size());
    std::memcpy(vec.data(), str.data(), str.size());
    return vec;
}

}  // namespace dorado::tests
