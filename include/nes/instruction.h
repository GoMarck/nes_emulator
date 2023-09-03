#pragma once

#include "nes/type.h"

#define OPCODE_SIZE 256

namespace nes {

/// Operation code.
enum Opcode : Byte {
  NUL = 0, 

  /// Logical and arithmetic code
  ORA,
  AND,
  EOR,
  ADC,
  SBC,
  CMP,
  CPX,
  CPY,
  DEC,
  DEX,
  DEY,
  INC,
  INX,
  INY,
  ASL,
  ROL,
  LSR,
  ROR,

  /// Move code
  LDA,
  STA,
  LDX,
  STX,
  LDY,
  STY,
  TAX,
  TXA,
  TAY,
  TYA,
  TSX,
  TXS,
  PLA,
  PHA,
  PLP,
  PHP,

  /// Jump/Flag code
  BPL,
  BMI,
  BVC,
  BVS,
  BCC,
  BCS,
  BNE,
  BEQ,
  BRK,
  RTI,
  JSR,
  RTS,
  JMP,
  BIT,
  CLC,
  SEC,
  CLD,
  SED,
  CLI,
  SEI,
  CLV,
  NOP,
};

enum AddressMode : Byte {
  IMP = 0,
  IMM,
  ZP,
  ZPX,
  ZPY,
  IZX,
  IZY,
  ABS,
  ABX,
  ABY,
  IND,
  REL,
};

};  // namespace nes