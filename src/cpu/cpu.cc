#include "cpu.h"

CPU::CPU(MMU& mmu_) : mmu(mmu_) {
    r8[0] = &registers.b;
    r8[1] = &registers.c;
    r8[2] = &registers.d;
    r8[3] = &registers.e;
    r8[4] = &registers.h;
    r8[5] = &registers.l;
    r8[6] = nullptr; // [HL]
    r8[7] = &registers.a;

    r16[0] = { &Registers::get_bc, &Registers::set_bc };
    r16[1] = { &Registers::get_de, &Registers::set_de };
    r16[2] = { &Registers::get_hl, &Registers::set_hl };
    r16[3] = { &Registers::get_sp, &Registers::set_sp };
}

void CPU::step() {
    u8 opcode = mmu.read_byte(registers.pc++);

    switch(opcode >> 6) {
        case 0b00: break;
        case 0b01: break;
        case 0b10: break;
        case 0b11: break;
    }
}

u8 CPU::alu_add(u8 lhs, u8 rhs, bool carry) {
    u16 result = lhs + rhs + (carry ? 1 : 0);

    registers.f.zero = ((result & 0xFF) == 0);
    registers.f.subtract = false;
    registers.f.half_carry = ((lhs & 0x0F) + (rhs & 0x0F) + carry) > 0x0F;
    registers.f.carry = result > 0xFF;

    return (u8)result;
}

u8 CPU::alu_sub(u8 lhs, u8 rhs, bool carry) {
    u16 result = lhs - rhs - (carry ? 1 : 0);

    registers.f.zero = ((result & 0xFF) == 0);
    registers.f.subtract = true;
    registers.f.half_carry = (lhs & 0x0F) < ((rhs & 0x0F) + carry);
    registers.f.carry = lhs < (rhs + carry);

    return (u8)result;
}

void CPU::inc_dec(u8 opcode) {
    u8 index = (opcode >> 3) & 0b111;
    bool dec = opcode & 0b1;

    // storing carry flag since ALU alters but INC/DEC do not
    bool old_carry = registers.f.carry;

    if(index == 6) {
        u16 addr = registers.get_hl();
        u8 value = mmu.read_byte(addr);
        value = dec ? alu_sub(value, 1, false) : alu_add(value, 1, false);
        mmu.write_byte(addr, value);
    } else {
        u8& reg = r8_ref(index);
        reg = dec ? alu_sub(reg, 1, false) : alu_add(reg, 1, false);
    }

    // restoring carry flag
    registers.f.carry = old_carry;
}

void CPU::inc_dec_r16(u8 opcode) {
    u8 index = (opcode >> 4) & 0b11;
    bool dec = opcode & 01;

    // storing carry flag since ALU alters but INC/DEC do not
    bool old_carry = registers.f.carry;

    u16 reg = (registers.*(r16[index].get))();
    reg = dec ? alu_sub(reg, 1, false) : alu_add(reg, 1, false);
    (registers.*(r16[index].set))(reg);

    // restoring carry flag
    registers.f.carry = old_carry;
}

void CPU::ld_r_r(u8 opcode) {
    // ld [hl], [hl], which yields halt
    if(opcode == 0x76) {
        // todo: implement halt
    }

    u8 dst = (opcode >> 3) & 0b111;
    u8 src = opcode & 0b111;

    u8 value  = r8_ref(src);
    r8_ref(dst) = value;
}
