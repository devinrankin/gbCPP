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

        u8 flags = 0;

        bool zero() const { return flags & FLAG_Z; }
        bool subtract() const { return flags & FLAG_N; }
        bool half_carry() const { return flags & FLAG_H; }
        bool carry() const { return flags & FLAG_C; }

        void set_zero(bool enabled) { enabled ? flags |= FLAG_Z : flags &= ~FLAG_Z; }
        void set_subtract(bool enabled) { enabled ? flags |= FLAG_N : flags &= ~FLAG_N; }
        void set_half_carry(bool enabled) { enabled ? flags |= FLAG_H : flags &= ~FLAG_H; }
        void set_carry(bool enabled) { enabled ? flags |= FLAG_C : flags &= ~FLAG_C; }

        u8 to_u8() const { return flags & 0xF0; }
        void from_u8(u8 value) { flags = value & 0xF0; }
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

        u16 get_bc() const { return static_cast<u16>(b) << 8 | static_cast<u16>(c); }
        u16 get_af() const { return static_cast<u16>(a) << 8 | static_cast<u16>(f.to_u8()); }
        u16 get_sp() const { return sp; }
        u16 get_de() const { return static_cast<u16>(d) << 8 | static_cast<u16>(e); }
        u16 get_hl() const { return static_cast<u16>(h) << 8 | static_cast<u16>(l); }

        void set_bc(u16 value) {
            b = static_cast<u8>((value & 0xFF00) >> 8);
            c = static_cast<u8>(value & 0x00FF);
        }
        void set_de(u16 value) {
            d = static_cast<u8>((value & 0xFF00) >> 8);
            e = static_cast<u8>(value & 0x00FF);
        }
        void set_hl(u16 value) {
            h = static_cast<u8>((value & 0xFF00) >> 8);
            l = static_cast<u8>(value & 0x00FF);
        }
        void set_af(u16 value) {
            a = static_cast<u8>((value & 0xFF00) >> 8);
            f.from_u8(static_cast<u8>(value & 0x00FF));
        }
        void set_sp(u16 value) {
            sp = value;
        }
    };

    struct RegisterPair {
        u16 (Registers::*get)(void) const;
        void (Registers::*set)(u16);
    };

    void step();
private:
    using OpHandler = void (CPU::*)(u8 opcode);
    Registers registers;
    u8* r8[8];
    RegisterPair r16[4];
    OpHandler opcode_table[256];
    bool halted;

    u8 read_r8(u8 index) {
        return index == 6 ? mmu.read_byte(registers.get_hl()) : *r8[index];
    }
    void write_r8(u8 index, u8 value) {
        if(index == 6) {
            mmu.write_byte(registers.get_hl(), value);
        } else {
            *r8[index] = value;
        }
    }

    void init_opcodes();
    void op_illegal(u8 opcode);

    u8 alu_add(u8 lhs, u8 rhs, bool carry);
    u8 alu_sub(u8 lhs, u8 rhs, bool carry);
    // ALU
    void alu_a_r8(u8 opcode);
    void alu_a_imm8(u8 opcode);
    void inc_dec(u8 opcode);
    void inc_dec_r16(u8 opcode);
    // LD
    void ld_r8_r8(u8 opcode);
    void ld_r8_imm8(u8 opcode);
    void ld_r16_imm16(u8 opcode);
    void ld_a(u8 opcode);
protected:
    MMU& mmu;
};

#endif
