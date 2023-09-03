#pragma once

#include "nes/type.h"

#define OPCODE_SIZE 256

namespace nes {

/// Operation code.
enum Opcode : Byte {
  NUL = 0, 

  /// Logical and arithmetic code
  ORA,  /// A:=A or {adr}
  AND,  /// A:=A&{adr}
  EOR,  /// A:=A exor {adr}
  ADC,  /// A:=A+{adr}
  SBC,  /// A:=A-{adr}
  CMP,  /// A-{adr}
  CPX,  /// X-{adr}
  CPY,  /// Y-{adr}
  DEC,  /// {adr}:={adr}-1
  DEX,  /// X:=X-1
  DEY,  /// Y:=Y-1
  INC,  /// {adr}:={adr}+1
  INX,  /// X:=X+1
  INY,  /// Y:=Y+1
  ASL,  /// {adr}:={adr}*2
  ROL,  /// {adr}:={adr}*2+C
  LSR,  /// {adr}:={adr}/2
  ROR,  /// {adr}:={adr}/2+C*128

  /// Move code
  LDA,  /// A:={adr}
  STA,  /// {adr}:=A
  LDX,  /// X:={adr}
  STX,  /// {adr}:=X
  LDY,  /// Y:={adr}
  STY,  /// {adr}:=Y
  TAX,  /// X:=A
  TXA,  /// A:=X
  TAY,  /// Y:=A
  TYA,  /// A:=Y
  TSX,  /// X:=S
  TXS,  /// S:=X
  PLA,  /// A:=+(S)
  PHA,  /// (S)-:=A
  PLP,  /// P:=+(S)
  PHP,  /// (S)-:=P

  /// Jump/Flag code
  BPL,  /// branch on N=0
  BMI,  /// branch on N=1
  BVC,  /// branch on V=0
  BVS,  /// branch on V=1
  BCC,  /// branch on C=0
  BCS,  /// branch on C=1
  BNE,  /// branch on Z=0
  BEQ,  /// branch on Z=1
  BRK,  /// (S)-:=PC,P PC:=($FFFE)
  RTI,  /// P,PC:=+(S)
  JSR,  /// (S)-:=PC PC:={adr}
  RTS,  /// PC:=+(S)
  JMP,  /// PC:={adr}
  BIT,  /// N:=b7 V:=b6 Z:=A&{adr}
  CLC,  /// C:=0
  SEC,  /// C:=1
  CLD,  /// D:=0
  SED,  /// D:=1
  CLI,  /// I:=0
  SEI,  /// I:=1
  CLV,  /// V:=0
  NOP,  /// 
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