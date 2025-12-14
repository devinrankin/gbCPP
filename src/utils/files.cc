#include <fstream>
#include "files.h"

std::vector<u8> read_bytes(const char* path) {
    std::ifstream file(path, std::ios::binary);

    file.seekg(0, std::ios::end);
    auto file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    auto buffer = std::vector<u8>(file_size);
    file.read(reinterpret_cast<char*>(buffer.data()), file_size);

    return buffer;
}
