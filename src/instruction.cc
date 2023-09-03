#include "nes/instruction.h"

namespace nes {

// clang-format off

/// Reference from: http://www.oxyron.de/html/opcodes02.html
Opcode opcode_matrix[OPCODE_SIZE] = {
    /// $00 ~ $0F
    BRK, ORA, NUL, NUL, NUL, ORA, ASL, NUL,
    PHP, ORA, ASL, NUL, NUL, ORA, ASL, NUL,
    /// $10 ~ $1F
    BPL, ORA, NUL, NUL, NUL, ORA, ASL, NUL,
    CLC, ORA, NUL, NUL, NUL, ORA, ASL, NUL,
    /// $20 ~ $2F
    JSR, AND, NUL, NUL, BIT, AND, ROL, NUL,
    PLP, AND, ROL, NUL, BIT, AND, ROL, NUL,
    /// $30 ~ $3F
    BMI, AND, NUL, NUL, NUL, AND, ROL, NUL,
    SEC, AND, NUL, NUL, NUL, AND, ROL, NUL,
    /// $40 ~ $4F
    RTI, EOR, NUL, NUL, NUL, EOR, LSR, NUL,
    PHA, EOR, LSR, NUL, JMP, EOR, LSR, NUL,
    /// $50 ~ $5F
    BVC, EOR, NUL, NUL, NUL, EOR, LSR, NUL,
    CLI, EOR, NUL, NUL, NUL, EOR, LSR, NUL,
    /// $60 ~ $6F
    RTS, ADC, NUL, NUL, NUL, ADC, ROR, NUL,
    PLA, ADC, ROR, NUL, JMP, ADC, ROR, NUL,
    /// $70 ~ $7F
    BVS, ADC, NUL, NUL, NUL, ADC, ROR, NUL,
    SEI, ADC, NUL, NUL, NUL, ADC, ROR, NUL,
    /// $80 ~ $8F
    NUL, STA, NUL, NUL, STY, STA, STX, NUL,
    DEY, NUL, TXA, NUL, STY, STA, STX, NUL,
    /// $90 ~ 9F
    BCC, STA, NUL, NUL, STY, STA, STX, NUL,
    TYA, STA, TXS, NUL, NUL, STA, NUL, NUL,
    /// $A0 ~ $AF
    LDY, LDA, LDX, NUL, LDY, LDA, LDX, NUL,
    CLV, LDA, TSX, NUL, LDY, LDA, LDX, NUL,
    /// $B0 ~ $BF
    BCS, LDA, NUL, NUL, LDY, LDA, LDX, NUL,
    CLV, LDA, TSX, NUL, LDY, LDA, LDX, NUL,
    /// $C0 ~ $CF
    CPY, CMP, NUL, NUL, CPY, CMP, DEC, NUL,
    INY, CMP, DEX, NUL, CPY, CMP, DEC, NUL,
    /// $D0 ~ $DF
    BNE, CMP, NUL, NUL, NUL, CMP, DEC, NUL,
    CLD, CMP, NUL, NUL, NUL, CMP, DEC, NUL,
    /// $E0 ~ $EF
    CPX, SBC, NUL, NUL, CPX, SBC, INC, NUL,
    INX, SBC, NOP, NUL, CPX, SBC, INC, NUL,
    /// $F0 ~ $FF
    BEQ, SBC, NUL, NUL, NUL, SBC, INC, NUL,
    SED, SBC, NUL, NUL, NUL, SBC, INC, NUL,
};

AddressMode opcode_addr_mode_matrix[OPCODE_SIZE] = {
    /// $00 ~ $0F
    IMP, IZX, IMP, IZX, ZP , ZP , ZP , ZP ,
    IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS,
    /// $10 ~ $1F
    REL, IZY, IMP, IZY, ZPX, ZPX, ZPX, ZPX,
    IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX,
    /// $20 ~ $2F
    ABS, IZX, IMP, IZX, ZP , ZP , ZP , ZP ,
    IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS,
    /// $30 ~ $3F
    REL, IZY, IMP, IZY, ZPX, ZPX, ZPX, ZPX,
    IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX,
    /// $40 ~ $4F
    IMP, IZX, IMP, IZX, ZP , ZP , ZP , ZP ,
    IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS,
    /// $50 ~ $5F
    REL, IZY, IMP, IZY, ZPX, ZPX, ZPX, ZPX,
    IMP, ABY, IMP, IMP, ABX, ABX, ABX, ABX,
    /// $60 ~ $6F
    IMP, IZX, IMP, IZX, ZP , ZP , ZP , ZP ,
    IMP, IMM, IMP, IMM, IND, ABS, ABS, ABS,
    /// $70 ~ $7F
    REL, IZY, IMP, IZY, ZPX, ZPX, ZPX, ZPX,
    IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX,
    /// $80 ~ $8F
    IMM, IZX, IMM, IZX, ZP , ZP , ZP , ZP ,
    IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS,
    /// $90 ~ $9F
    REL, IZY, IMP, IZY, ZPX, ZPX, ZPY, ZPY,
    IMP, ABY, IMP, ABY, ABX, ABX, ABY, ABY,
    /// $A0 ~ $AF
    IMM, IZX, IMM, IZX, ZP , ZP , ZP , ZP ,
    IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS,
    /// $B0 ~ $BF
    REL, IZY, IMP, IZY, ZPX, ZPX, ZPY, ZPY,
    IMP, ABY, IMP, ABY, ABX, ABX, ABY, ABY,
    /// $C0 ~ $CF
    IMM, IZX, IMM, IZX, ZP , ZP , ZP , ZP ,
    IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS,
    /// $D0 ~ $DF
    REL, IZY, IMP, IZY, ZPX, ZPX, ZPX, ZPX,
    IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX,
    /// $E0 ~ $EF
    IMM, IZX, IMM, IZX, ZP , ZP , ZP , ZP ,
    IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS,
    /// $F0 ~ $FF
    REL, IZY, IMP, IZY, ZPX, ZPX, ZPX, ZPX,
    IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX,
};

bool opcode_addtional_cycle_matrix[OPCODE_SIZE] = {
    /// $00 ~ $0F
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /// $10 ~ $1F
    true , true , false, false, false, false, false, false,
    false, true , false, false, true , true , false, false,
    /// $20 ~ $2F
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /// $30 ~ $3F
    true , true , false, false, false, false, false, false,
    false, true , false, false, true , true , false, false,
    /// $40 ~ $4F
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /// $50 ~ $5F
    true , true , false, false, false, false, false, false,
    false, true , false, false, true , true , false, false,
    /// $60 ~ $6F
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /// $70 ~ $7F
    true , true , false, false, false, false, false, false,
    false, true , false, false, true , true , false, false,
    /// $80 ~ $8F
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /// $90 ~ $9F
    true , false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /// $A0 ~ $AF
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /// $B0 ~ $BF
    true , true , false, true , false, false, false, false,
    false, true , false, true , true , true , true , true ,
    /// $C0 ~ $CF
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /// $D0 ~ $DF
    true , true , false, false, false, false, false, false,
    false, true , false, false, true , true , false, false,
    /// $E0 ~ $EF
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    /// $F0 ~ $FF
    true , true , false, false, false, false, false, false,
    false, true , false, false, true , true , false, false, 
};

Byte opcode_cycle_matrix[OPCODE_SIZE] = {
    7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
    6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0,
    2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0,
    2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0,
    2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0,
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
};

// clang-format on

}  // namespace nes