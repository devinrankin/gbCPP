#include <fstream>
#include "mmu.h"

void MMU::load_rom(const char* path) {
    std::ifstream file(path, std::ios::binary);

    file.seekg(0, std::ios::end);
    auto file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    rom = std::vector<u8>(file_size);
    file.read(reinterpret_cast<char*>(rom.data()), file_size);
}

u8 MMU::read_byte(u16 addr) const {
    u8 byte = rom[addr];
    return byte;
}

void MMU::write_byte(u16 addr, u8 value) {
    rom[addr] = value;
}
