#include "cpu.h"
#include "../utils/logging.h"

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

    // the first two bits of an opcode can be grouped into four families
    // https://gbdev.io/pandocs/CPU_Instruction_Set.html
    switch(opcode >> 6) {
        case 0b00: break;
        case 0b01: break;
        case 0b10: break;
        case 0b11: break;
    }
}

u8 CPU::alu_add(u8 lhs, u8 rhs, bool carry) {
    u16 result = lhs + rhs + (carry ? 1 : 0);

    registers.f.set_zero(((result & 0xFF) == 0));
    registers.f.set_subtract(false);
    registers.f.set_half_carry(((lhs & 0x0F) + (rhs & 0x0F) + carry) > 0x0F);
    registers.f.set_carry(result > 0xFF);

    return static_cast<u8>(result);
}

u8 CPU::alu_sub(u8 lhs, u8 rhs, bool carry) {
    u16 result = lhs - rhs - (carry ? 1 : 0);

    registers.f.set_zero(((result & 0xFF) == 0));
    registers.f.set_subtract(true);
    registers.f.set_half_carry(((lhs & 0x0F) + (rhs & 0x0F) + carry) > 0x0F);
    registers.f.set_carry(result > 0xFF);

    return static_cast<u8>(result);
}

void CPU::op_illegal(u8 opcode) {
    halted = true;
    // todo: implement logging utility
    LOG_ERROR(
        "Illegal opcode %02X at pc=%04X",
        opcode,
        registers.pc - 1
    );
}

void CPU::inc_dec(u8 opcode) {
    u8 operand = (opcode >> 3) & 0b111;
    bool dec = opcode & 0b1;

    u8 value = read_r8(operand);
    if(dec) {
        registers.f.set_subtract(true);
        registers.f.set_half_carry((value & 0x0F) == 0x00);
        value--;
    } else {
        registers.f.set_subtract(false);
        registers.f.set_half_carry((value & 0x0F) == 0x0F);
        value++;
    }
    registers.f.set_zero(value == 0);

    write_r8(operand, value);
}

void CPU::inc_dec_r16(u8 opcode) {
    u8 operand = (opcode >> 4) & 0b11;
    bool dec = opcode & 0b1;

    u16 value = (registers.*(r16[operand].get))();
    dec ? value-- : value++;

    (registers.*(r16[operand].set))(value);
}

void CPU::ld_r8_r8(u8 opcode) {
    // ld [hl], [hl], which yields halt
    if(opcode == 0x76) {
        // todo: implement halt
    }

    u8 dst = (opcode >> 3) & 0b111;
    u8 src = opcode & 0b111;

    write_r8(dst, read_r8(src));
}

void CPU::ld_r8_imm8(u8 opcode) {
    u8 dst = (opcode >> 3) & 0b111;
    u8 value = mmu.read_byte(registers.pc++);

    write_r8(read_r8(dst), value);
}

void CPU::ld_r16_imm16(u8 opcode) {
    u8 dst = (opcode >> 4) & 0b11;
    u16 value = mmu.read_byte(registers.pc++) << 8 | mmu.read_byte(registers.pc++);

    (registers.*(r16[dst].set))(value);
}

void CPU::ld_a(u8 opcode) {
    u16 reg = (opcode >> 4) & 0b11;
    u16 addr = (registers.*(r16[reg].get))();

    // this bit determines if the 16-bit register indirect address is the source or dest
    bool is_src = (opcode >> 3) & 0b1;

    if(is_src) {
        registers.a = mmu.read_byte(addr);
    } else {
        mmu.write_byte(addr, registers.a);
    }
}

// todo: n8, r8, and imm8 are similar enough to create grouped functions of
void CPU::alu_a_r8(u8 opcode) {
    u8& accumulator = registers.a;
    u8 rhs = read_r8(opcode & 0b111);
    u8 alu_op = (opcode >> 3) & 0b111;
    bool carry = registers.f.carry();

    switch(alu_op) {
        case 0:
            accumulator = alu_add(accumulator, rhs, false);
            break;
        case 1:
            accumulator = alu_add(accumulator, rhs, carry);
            break;
        case 2:
            accumulator = alu_sub(accumulator, rhs, false);
            break;
        case 3:
            accumulator = alu_sub(accumulator, rhs, carry);
            break;
        case 4:
            accumulator &= rhs;
            registers.f.set_zero(accumulator == 0);
            registers.f.set_subtract(false);
            registers.f.set_half_carry(true);
            registers.f.set_carry(false);
            break;
        case 5:
            accumulator ^= rhs;
            registers.f.set_zero(accumulator == 0);
            registers.f.set_subtract(false);
            registers.f.set_half_carry(false);
            registers.f.set_carry(false);
            break;
        case 6:
            accumulator |= rhs;
            registers.f.set_zero(accumulator == 0);
            registers.f.set_subtract(false);
            registers.f.set_half_carry(false);
            registers.f.set_carry(false);
            break;
        case 7:
            u8 discarded = alu_sub(accumulator, rhs, false);
            break;
    }
}

void CPU::init_opcodes() {
    for (OpHandler& fn : opcode_table) {
        fn = &CPU::op_illegal;
    }

    // ld r16, imm16
    for(u8 reg = 0; reg < 4; reg++) {
        opcode_table[(reg << 4) | 0x01] = &CPU::ld_r16_imm16;
    }

    // ld r8, imm8
    // ld r8, r8
    for(u8 reg = 0; reg < 8; reg++) {
        opcode_table[(reg << 3) | 0x06] = &CPU::ld_r8_imm8;
    }

    for(u8 reg = 0; reg < 8; reg++) {
        opcode_table[0x40 + reg] = &CPU::ld_r8_r8;
    }

    for(u8 reg = 0; reg < 8; reg++) {
        opcode_table[0x70 + reg] = &CPU::ld_r8_r8;
    }

    // inc r8 ; dec r8
    // inc r18; dec r16
    for(int reg = 0; reg < 4; reg++) {
        opcode_table[(reg << 4) | 0x03] = &CPU::inc_dec;
        opcode_table[(reg << 4) | 0x0B] = &CPU::inc_dec;
    }

    for(int reg = 0; reg < 8; reg++) {
        opcode_table[(reg << 3) | 0x04] = &CPU::inc_dec;
        opcode_table[(reg << 3) | 0x05] = &CPU::inc_dec;
    }

    // alu ops (e.g. add a, r8 ; add a, imm8)
    for(int op = 0; op < 8; op++) {
        for(int reg = 0; reg < 8; reg++) {
            opcode_table[0x40 | (op << 3) | reg] = &CPU::alu_a_r8;
        }
    }

    for(int op = 0; op < 8; op++) {
        opcode_table[0x40 | (op << 3) | 0x06] = &CPU::alu_a_imm8;
    }
}
