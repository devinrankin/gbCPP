#ifndef MEM_H
#define MEM_H
#include <vector>
#include "../utils/types.h"

class MMU {
public:
    MMU();
    void load_rom(const char* path);
    u8 read_byte(u16 addr) const;
    void write_byte(u16 addr, u8 value);

private:
    std::vector<u8> rom;
};

#endif
