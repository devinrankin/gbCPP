#ifndef CPU_H
#define CPU_H
#include "../utils/types.h"
#include "../mem/mmu.h"

class CPU {
public:
    CPU(MMU& mmu_);

    struct FlagsRegister {
        static constexpr u8 FLAG_Z = 0x80;
        static constexpr u8 FLAG_N = 0x40;
        static constexpr u8 FLAG_H = 0x20;
        static constexpr u8 FLAG_C = 0x10;

        bool zero = false;
        bool subtract = false;
        bool half_carry = false;
        bool carry = false;

        u8 to_u8() const {
            u8 flags_byte = 0;
            if(zero) { flags_byte |= FLAG_Z; }
            if(subtract) { flags_byte |= FLAG_N; }
            if(half_carry) { flags_byte |= FLAG_H; }
            if(carry) { flags_byte |= FLAG_C; }

            return flags_byte;
        }
        void from_u8(u8 byte) {
            byte &= 0xF0; // flag register only cares about the upper nibble, so we 0 the lower.
            zero = (byte & FLAG_Z) != 0;
            subtract = (byte & FLAG_N) != 0;
            half_carry = (byte & FLAG_H) != 0;
            carry = (byte & FLAG_C) != 0;
        }
    };

    struct Registers {
        u8 b;
        u8 c;
        u8 d;
        u8 e;
        u8 h;
        u8 l;
        u8 a;
        FlagsRegister f;
        u16 sp;
        u16 pc;

        u16 get_bc() const {
            return (u16)b << 8 | (u16)c;
        }
        void set_bc(u16 value) {
            b = (u8)((value & 0xFF00) >> 8);
            c = (u8)(value & 0x00FF);
        }

        u16 get_de() const {
            return (u16)d << 8 | (u16)e;
        }
        void set_de(u16 value) {
            d = (u8)((value & 0xFF00) >> 8);
            e = (u8)(value & 0x00FF);
        }

        u16 get_hl() const {
            return (u16)h << 8 | (u16)l;
        }
        void set_hl(u16 value) {
            h = (u8)((value & 0xFF00) >> 8);
            l = (u8)(value & 0x00FF);
        }

        u16 get_af() const {
            return (u16)a << 8 | (u16)f.to_u8();
        }
        void set_af(u16 value) {
            a = (u8)((value & 0xFF00) >> 8);
            f.from_u8((u8)(value & 0x00FF));
        }
    };

    void step();

private:
    Registers registers;
    void INC_DEC_r8(u8 opcode);
    void INC_DEC_r16(u8 opcode);
    void LD_r8(u8 opcode);
    void LD_r16(u8 opcode);
protected:
    MMU& mmu;
};

#endif
