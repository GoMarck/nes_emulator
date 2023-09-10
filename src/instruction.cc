#include "nes/instruction.h"

#include "nes/logging.h"
#include "nes/main_bus.h"
#include "nes/type.h"

namespace nes {

void ORA_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void AND_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void EOR_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void ADC_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void SBC_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void CMP_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void CPX_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void CPY_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void DEC_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void DEX_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void DEY_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void INC_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void INX_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void INY_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void ASL_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void ROL_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void LSR_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void ROR_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void LDA_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void STA_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void LDX_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void STX_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void LDY_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void STY_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void TAX_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void TXA_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void TAY_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void TYA_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void TSX_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void TXS_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void PLA_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void PHA_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void PLP_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void PHP_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void BPL_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void BMI_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void BVC_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void BVS_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void BCC_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void BCS_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void BNE_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void BEQ_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void BRK_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void RTI_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void JSR_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void RTS_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void JMP_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void BIT_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void CLC_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void SEC_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void CLD_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void SED_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void CLI_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void SEI_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void CLV_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);
void NOP_handler(Byte code, CPU *cpu, MainBus *bus, Word operand,
                 AddressMode mode);

void InitOpcodeHandlers(std::unordered_map<Byte, OpcodeHandler> &handlers) {
  for (Byte i = 0; i < 0xFF; ++i) {
    Opcode code = opcode_matrix[i];
    switch (code) {
      case NUL:
        continue;
      case ORA:
        handlers.emplace(code, ORA_handler);
        break;
      case AND:
        handlers.emplace(code, AND_handler);
        break;
      case EOR:
        handlers.emplace(code, EOR_handler);
        break;
      case ADC:
        handlers.emplace(code, ADC_handler);
        break;
      case SBC:
        handlers.emplace(code, SBC_handler);
        break;
      case CMP:
        handlers.emplace(code, CMP_handler);
        break;
      case CPX:
        handlers.emplace(code, CPX_handler);
        break;
      case CPY:
        handlers.emplace(code, CPY_handler);
        break;
      case DEC:
        handlers.emplace(code, DEC_handler);
        break;
      case DEX:
        handlers.emplace(code, DEX_handler);
        break;
      case DEY:
        handlers.emplace(code, DEY_handler);
        break;
      case INC:
        handlers.emplace(code, INC_handler);
        break;
      case INX:
        handlers.emplace(code, INX_handler);
        break;
      case INY:
        handlers.emplace(code, INY_handler);
        break;
      case ASL:
        handlers.emplace(code, ASL_handler);
        break;
      case ROL:
        handlers.emplace(code, ROL_handler);
        break;
      case LSR:
        handlers.emplace(code, LSR_handler);
        break;
      case ROR:
        handlers.emplace(code, ROR_handler);
        break;
      case LDA:
        handlers.emplace(code, LDA_handler);
        break;
      case STA:
        handlers.emplace(code, STA_handler);
        break;
      case LDX:
        handlers.emplace(code, LDX_handler);
        break;
      case STX:
        handlers.emplace(code, STX_handler);
        break;
      case LDY:
        handlers.emplace(code, LDY_handler);
        break;
      case STY:
        handlers.emplace(code, STY_handler);
        break;
      case TAX:
        handlers.emplace(code, TAX_handler);
        break;
      case TXA:
        handlers.emplace(code, TXA_handler);
        break;
      case TAY:
        handlers.emplace(code, TAY_handler);
        break;
      case TYA:
        handlers.emplace(code, TYA_handler);
        break;
      case TSX:
        handlers.emplace(code, TSX_handler);
        break;
      case TXS:
        handlers.emplace(code, TXS_handler);
        break;
      case PLA:
        handlers.emplace(code, PLA_handler);
        break;
      case PHA:
        handlers.emplace(code, PHA_handler);
        break;
      case PLP:
        handlers.emplace(code, PLP_handler);
        break;
      case PHP:
        handlers.emplace(code, PHP_handler);
        break;
      case BPL:
        handlers.emplace(code, BPL_handler);
        break;
      case BMI:
        handlers.emplace(code, BMI_handler);
        break;
      case BVC:
        handlers.emplace(code, BVC_handler);
        break;
      case BVS:
        handlers.emplace(code, BVS_handler);
        break;
      case BCC:
        handlers.emplace(code, BCC_handler);
        break;
      case BCS:
        handlers.emplace(code, BCS_handler);
        break;
      case BNE:
        handlers.emplace(code, BNE_handler);
        break;
      case BEQ:
        handlers.emplace(code, BEQ_handler);
        break;
      case BRK:
        handlers.emplace(code, BRK_handler);
        break;
      case RTI:
        handlers.emplace(code, RTI_handler);
        break;
      case JSR:
        handlers.emplace(code, JSR_handler);
        break;
      case RTS:
        handlers.emplace(code, RTS_handler);
        break;
      case JMP:
        handlers.emplace(code, JMP_handler);
        break;
      case BIT:
        handlers.emplace(code, BIT_handler);
        break;
      case CLC:
        handlers.emplace(code, CLC_handler);
        break;
      case SEC:
        handlers.emplace(code, SEC_handler);
        break;
      case CLD:
        handlers.emplace(code, CLD_handler);
        break;
      case SED:
        handlers.emplace(code, SED_handler);
        break;
      case CLI:
        handlers.emplace(code, CLI_handler);
        break;
      case SEI:
        handlers.emplace(code, SEI_handler);
        break;
      case CLV:
        handlers.emplace(code, CLV_handler);
        break;
      case NOP:
        handlers.emplace(code, NOP_handler);
        break;
      default:
        NES_LOG(FATAL) << "Unrecognized opcode: " << code;
        break;
    }
  }
}

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