#include "nes/instruction.h"

#include <glog/logging.h>
#include <sys/errno.h>

#include "nes/cpu.h"
#include "nes/logging.h"
#include "nes/main_bus.h"
#include "nes/type.h"

namespace nes {

bool IsPageCrossed(Address address, Word increament) {
  return (address & 0xFF00) != ((address + increament) & 0xFF00);
}

bool ParseImmediateOperand(CPU *cpu, MainBus *bus, Byte *operand) {
  *operand = bus->Read(cpu->GetProgramCounter());
  cpu->IncreaseProgramCounter(1);
  return false;
}

Address ParseZeroPageAddress(CPU *cpu, MainBus *bus) {
  cpu->IncreaseProgramCounter(1);
  return bus->Read(cpu->GetProgramCounter());
}

Address ParseZeroPageXAddress(CPU *cpu, MainBus *bus) {
  cpu->IncreaseProgramCounter(1);
  return bus->Read(cpu->GetProgramCounter()) + cpu->GetX();
}

Address ParseZeroPageYAddress(CPU *cpu, MainBus *bus) {
  cpu->IncreaseProgramCounter(1);
  return bus->Read(cpu->GetProgramCounter()) + cpu->GetY();
}

Address ParseAbsoluteAddress(CPU *cpu, MainBus *bus) {
  cpu->IncreaseProgramCounter(2);
  return (static_cast<Address>(bus->Read(cpu->GetProgramCounter() + 1))
          << 0x8) +
         bus->Read(cpu->GetProgramCounter());
}

Address ParseAbsoluteXAddress(CPU *cpu, MainBus *bus, bool *page_crossed) {
  cpu->IncreaseProgramCounter(2);
  Address address =
      (static_cast<Address>(bus->Read(cpu->GetProgramCounter() + 1)) << 0x8) +
      bus->Read(cpu->GetProgramCounter());
  *page_crossed = IsPageCrossed(address, cpu->GetX());
  address += cpu->GetX();
  return address;
}

Address ParseAbsoulteYAddress(CPU *cpu, MainBus *bus, bool *page_crossed) {
  cpu->IncreaseProgramCounter(2);
  Address address =
      (static_cast<Address>(bus->Read(cpu->GetProgramCounter() + 1)) << 0x8) +
      bus->Read(cpu->GetProgramCounter());
  *page_crossed = IsPageCrossed(address, cpu->GetY());
  address += cpu->GetY();
  return address;
}

Address ParseIndexedZeroPageXAddress(CPU *cpu, MainBus *bus) {
  cpu->IncreaseProgramCounter(1);
  Byte index_address = bus->Read(cpu->GetProgramCounter()) + cpu->GetX();
  return (static_cast<Address>(bus->Read(index_address + 1)) << 0x8) +
         bus->Read(index_address);
}

Address ParseIndexedZeroPageYAddress(CPU *cpu, MainBus *bus,
                                     bool *page_crossed) {
  cpu->IncreaseProgramCounter(1);
  Byte index_address = bus->Read(cpu->GetProgramCounter());
  Address address =
      (static_cast<Address>(bus->Read(index_address + 1)) << 0x8) +
      bus->Read(index_address);
  *page_crossed = IsPageCrossed(address, cpu->GetY());
  address += cpu->GetY();
  return address;
}

Address ParseAddress(AddressMode mode, CPU *cpu, MainBus *bus,
                     bool *page_crossed) {
  *page_crossed = false;
  switch (mode) {
    case AddressMode::ABY:
      return ParseAbsoulteYAddress(cpu, bus, page_crossed);
    case AddressMode::ABX:
      return ParseAbsoluteXAddress(cpu, bus, page_crossed);
    case AddressMode::ABS:
      return ParseAbsoluteAddress(cpu, bus);
    case AddressMode::IZY:
      return ParseIndexedZeroPageYAddress(cpu, bus, page_crossed);
    case AddressMode::IZX:
      return ParseIndexedZeroPageXAddress(cpu, bus);
    case AddressMode::ZPY:
      return ParseZeroPageYAddress(cpu, bus);
    case AddressMode::ZPX:
      return ParseZeroPageXAddress(cpu, bus);
    case AddressMode::ZP:
      return ParseZeroPageAddress(cpu, bus);
    case AddressMode::IMP:
    case AddressMode::REL:
    case AddressMode::IND:
    case AddressMode::IMM:
    default:
      LOG(FATAL) << "Unrecognized or invalid address mode: " << mode;
  }
}

bool ParseOperand(Byte code, CPU *cpu, MainBus *bus, Byte *operand) {
  AddressMode mode = opcode_addr_mode_matrix[code];
  if (mode == AddressMode::IMP || mode == AddressMode::REL ||
      mode == AddressMode::IND) {
    return false;
  }
  if (mode == IMM) {
    return ParseImmediateOperand(cpu, bus, operand);
  }

  bool page_crossed;
  Address address = ParseAddress(mode, cpu, bus, &page_crossed);
  *operand = bus->Read(address);
  return page_crossed;
}

Byte CalculateCycles(Byte code, bool page_crossed) {
  Byte cycles = opcode_cycle_matrix[code];
  CHECK(cycles > 0) << "Invalid zero cycle happen, code: " << code;
  if (page_crossed && opcode_addtional_cycle_matrix[code]) {
    cycles += 1;
  }
  return cycles - 1;
}

bool IsNegative(Byte data) { return (data & 0x80) != 0; }

Byte ORA_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::ORA) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  Byte a = cpu->GetA();
  Byte operand;
  bool page_crossed = ParseOperand(code, cpu, bus, &operand);
  a |= operand;
  cpu->SetA(a);
  if (a == 0) {
    cpu->SetZeroFlag();
  } else if (IsNegative(a)) {
    cpu->SetNegativeFlag();
  }
  return CalculateCycles(code, page_crossed);
}

Byte AND_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::AND) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  Byte a = cpu->GetA();
  Byte operand;
  bool page_crossed = ParseOperand(code, cpu, bus, &operand);
  a &= operand;
  cpu->SetA(a);
  if (a == 0) {
    cpu->SetZeroFlag();
  } else if (IsNegative(a)) {
    cpu->SetNegativeFlag();
  }
  return CalculateCycles(code, page_crossed);
}

Byte EOR_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::EOR) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  Byte a = cpu->GetA();
  Byte operand;
  bool page_crossed = ParseOperand(code, cpu, bus, &operand);
  a ^= operand;
  cpu->SetA(a);
  if (a == 0) {
    cpu->SetZeroFlag();
  } else if (IsNegative(a)) {
    cpu->SetNegativeFlag();
  }
  return CalculateCycles(code, page_crossed);
}

Byte ADC_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::ADC) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  Byte operand;
  bool page_crossed = ParseOperand(code, cpu, bus, &operand);
  Byte a = cpu->GetA();
  Byte result = a + operand + static_cast<Byte>(cpu->CarryFlag());

  if (result < a) {
    cpu->SetCarryFlag();
  }

  if (((result & 0x80) ^ (a & 0x80)) != 0) {
    cpu->SetOverflowFlag();
  }

  if (result == 0) {
    cpu->SetZeroFlag();
  }

  if (IsNegative(result)) {
    cpu->SetNegativeFlag();
  }

  return CalculateCycles(code, page_crossed);
}

Byte SBC_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::SBC) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  Byte operand;
  bool page_crossed = ParseOperand(code, cpu, bus, &operand);
  Byte a = cpu->GetA();
  Byte result = a - operand - (1 - static_cast<Byte>(cpu->CarryFlag()));

  if (result < a) {
    cpu->SetCarryFlag();
  }

  if (((result & 0x80) ^ (a & 0x80)) != 0) {
    cpu->SetOverflowFlag();
  }

  if (result == 0) {
    cpu->SetZeroFlag();
  }

  if (IsNegative(result)) {
    cpu->SetNegativeFlag();
  }

  return CalculateCycles(code, page_crossed);
}

Byte CMP_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::CMP) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  Byte operand;
  bool page_crossed = ParseOperand(code, cpu, bus, &operand);
  Byte result = cpu->GetA() - operand;
  if (IsNegative(result)) {
    cpu->SetNegativeFlag();
  }
  if (cpu->GetA() >= operand) {
    cpu->SetCarryFlag();
  }
  if (cpu->GetA() == operand) {
    cpu->SetZeroFlag();
  }
  return CalculateCycles(code, page_crossed);
}

Byte CPX_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::CPX) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  Byte operand;
  bool page_crossed = ParseOperand(code, cpu, bus, &operand);
  Byte result = cpu->GetX() - operand;
  if (IsNegative(result)) {
    cpu->SetNegativeFlag();
  }
  if (cpu->GetX() >= operand) {
    cpu->SetCarryFlag();
  }
  if (cpu->GetX() == operand) {
    cpu->SetZeroFlag();
  }
  return CalculateCycles(code, page_crossed);
}

Byte CPY_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::CPY) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  Byte operand;
  bool page_crossed = ParseOperand(code, cpu, bus, &operand);
  Byte result = cpu->GetY() - operand;
  if (IsNegative(result)) {
    cpu->SetNegativeFlag();
  }
  if (cpu->GetY() >= operand) {
    cpu->SetCarryFlag();
  }
  if (cpu->GetY() == operand) {
    cpu->SetZeroFlag();
  }
  return CalculateCycles(code, page_crossed);
}

Byte DEC_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::DEC) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  bool page_crossed;
  Address address =
      ParseAddress(opcode_addr_mode_matrix[code], cpu, bus, &page_crossed);
  Byte result = bus->Read(address) - 1;
  bus->Write(address, result);
  if (result == 0) {
    cpu->SetZeroFlag();
  } else if (IsNegative(result)) {
    cpu->SetNegativeFlag();
  }
  return CalculateCycles(code, page_crossed);
}

Byte DEX_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::DEX) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->SetX(cpu->GetX() - 1);
  if (cpu->GetX() == 0) {
    cpu->SetZeroFlag();
  } else if (IsNegative(cpu->GetX())) {
    cpu->SetNegativeFlag();
  }
  return CalculateCycles(code, false);
}

Byte DEY_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::DEY) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->SetY(cpu->GetY() - 1);
  if (cpu->GetY() == 0) {
    cpu->SetZeroFlag();
  } else if (IsNegative(cpu->GetY())) {
    cpu->SetNegativeFlag();
  }
  return CalculateCycles(code, false);
}

Byte INC_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::INC) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  bool page_crossed;
  Address address =
      ParseAddress(opcode_addr_mode_matrix[code], cpu, bus, &page_crossed);
  Byte result = bus->Read(address) + 1;
  bus->Write(address, result);
  if (result == 0) {
    cpu->SetZeroFlag();
  } else if (IsNegative(result)) {
    cpu->SetNegativeFlag();
  }
  return CalculateCycles(code, page_crossed);
}

Byte INX_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::INX) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->SetX(cpu->GetX() + 1);
  if (cpu->GetX() == 0) {
    cpu->SetZeroFlag();
  } else if (IsNegative(cpu->GetX())) {
    cpu->SetNegativeFlag();
  }
  return CalculateCycles(code, false);
}

Byte INY_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::INY) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->SetY(cpu->GetY() + 1);
  if (cpu->GetY() == 0) {
    cpu->SetZeroFlag();
  } else if (IsNegative(cpu->GetY())) {
    cpu->SetNegativeFlag();
  }
  return CalculateCycles(code, false);
}

Byte ASL_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::ASL) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  AddressMode mode = opcode_addr_mode_matrix[code];
  Byte old_val;
  Byte result;
  bool page_crossed = false;
  if (mode == AddressMode::IMP) {
    old_val = cpu->GetA();
    result = old_val << 1;
    cpu->SetA(result);
  } else {
    Address address = ParseAddress(mode, cpu, bus, &page_crossed);
    old_val = bus->Read(address);
    result = old_val << 1;
    bus->Write(address, result);
  }

  if (IsNegative(old_val)) {
    cpu->SetCarryFlag();
  } else {
    cpu->ClearCarryFlag();
  }

  if (cpu->GetA() == 0) {
    cpu->SetZeroFlag();
  }

  if (IsNegative(result)) {
    cpu->SetNegativeFlag();
  }

  return CalculateCycles(code, page_crossed);
}

Byte ROL_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::ROL) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  AddressMode mode = opcode_addr_mode_matrix[code];
  Byte old_val;
  Byte result;
  bool page_crossed = false;
  if (mode == AddressMode::IMP) {
    old_val = cpu->GetA();
    result = old_val << 1 | static_cast<Byte>(cpu->CarryFlag());
    cpu->SetA(result);
  } else {
    Address address = ParseAddress(mode, cpu, bus, &page_crossed);
    old_val = bus->Read(address);
    result = old_val << 1 | static_cast<Byte>(cpu->CarryFlag());
    bus->Write(address, result);
  }

  if (IsNegative(old_val)) {
    cpu->SetCarryFlag();
  } else {
    cpu->ClearCarryFlag();
  }

  if (cpu->GetA() == 0) {
    cpu->SetZeroFlag();
  }

  if (IsNegative(result)) {
    cpu->SetNegativeFlag();
  }

  return CalculateCycles(code, page_crossed);
}

Byte LSR_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::LSR) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  AddressMode mode = opcode_addr_mode_matrix[code];
  Byte old_val;
  Byte result;
  bool page_crossed = false;
  if (mode == AddressMode::IMP) {
    old_val = cpu->GetA();
    result = old_val >> 1;
    cpu->SetA(result);
  } else {
    Address address = ParseAddress(mode, cpu, bus, &page_crossed);
    old_val = bus->Read(address);
    result = old_val >> 1;
    bus->Write(address, result);
  }

  if ((old_val & 0x1) == 0) {
    cpu->ClearCarryFlag();
  } else {
    cpu->SetCarryFlag();
  }

  if (cpu->GetA() == 0) {
    cpu->SetZeroFlag();
  }

  if (IsNegative(result)) {
    cpu->SetNegativeFlag();
  }

  return CalculateCycles(code, page_crossed);
}

Byte ROR_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::ROR) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  AddressMode mode = opcode_addr_mode_matrix[code];
  Byte old_val;
  Byte result;
  bool page_crossed = false;
  if (mode == AddressMode::IMP) {
    old_val = cpu->GetA();
    result = old_val >> 1 | (static_cast<Byte>(cpu->CarryFlag()) << 0x7);
    cpu->SetA(result);
  } else {
    Address address = ParseAddress(mode, cpu, bus, &page_crossed);
    old_val = bus->Read(address);
    result = old_val << 1 | (static_cast<Byte>(cpu->CarryFlag()) << 0x7);
    bus->Write(address, result);
  }

  if ((old_val & 0x1) == 0) {
    cpu->ClearCarryFlag();
  } else {
    cpu->SetCarryFlag();
  }

  if (cpu->GetA() == 0) {
    cpu->SetZeroFlag();
  }

  if (IsNegative(result)) {
    cpu->SetNegativeFlag();
  }

  return CalculateCycles(code, page_crossed);
}

Byte LDA_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::LDA) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  Byte operand;
  bool page_crossed = ParseOperand(code, cpu, bus, &operand);
  cpu->SetA(operand);
  return CalculateCycles(code, page_crossed);
}

Byte STA_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::STA) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  bool page_crossed;
  Address address =
      ParseAddress(opcode_addr_mode_matrix[code], cpu, bus, &page_crossed);
  bus->Write(address, cpu->GetA());
  return CalculateCycles(code, page_crossed);
}

Byte LDX_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::LDA) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  Byte operand;
  bool page_crossed = ParseOperand(code, cpu, bus, &operand);
  cpu->SetX(operand);
  return CalculateCycles(code, page_crossed);
}

Byte STX_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::STX) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  bool page_crossed;
  Address address =
      ParseAddress(opcode_addr_mode_matrix[code], cpu, bus, &page_crossed);
  bus->Write(address, cpu->GetX());
  return CalculateCycles(code, page_crossed);
}

Byte LDY_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::LDY) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  Byte operand;
  bool page_crossed = ParseOperand(code, cpu, bus, &operand);
  cpu->SetY(operand);
  return CalculateCycles(code, page_crossed);
}

Byte STY_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::STY) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  bool page_crossed;
  Address address =
      ParseAddress(opcode_addr_mode_matrix[code], cpu, bus, &page_crossed);
  bus->Write(address, cpu->GetY());
  return CalculateCycles(code, page_crossed);
}

Byte TAX_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::TAX) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->SetX(cpu->GetA());
  auto x = cpu->GetX();
  if (x == 0) {
    cpu->SetZeroFlag();
  } else if (IsNegative(x)) {
    cpu->SetNegativeFlag();
  }
  return CalculateCycles(code, false);
}

Byte TXA_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::TXA) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->SetA(cpu->GetX());
  auto a = cpu->GetA();
  if (a == 0) {
    cpu->SetZeroFlag();
  } else if (IsNegative(a)) {
    cpu->SetNegativeFlag();
  }
  return CalculateCycles(code, false);
}

Byte TAY_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::TAY) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->SetY(cpu->GetA());
  auto y = cpu->GetY();
  if (y == 0) {
    cpu->SetZeroFlag();
  } else if (IsNegative(y)) {
    cpu->SetNegativeFlag();
  }
  return CalculateCycles(code, false);
}

Byte TYA_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::TXA) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->SetA(cpu->GetY());
  auto a = cpu->GetA();
  if (a == 0) {
    cpu->SetZeroFlag();
  } else if (IsNegative(a)) {
    cpu->SetNegativeFlag();
  }
  return CalculateCycles(code, false);
}

Byte TSX_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::TSX) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->SetX(cpu->GetStackPointer());
  auto x = cpu->GetX();
  if (x == 0) {
    cpu->SetZeroFlag();
  } else if (IsNegative(x)) {
    cpu->SetNegativeFlag();
  }
  return CalculateCycles(code, false);
}

Byte TXS_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::TXS) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->SetStackPointer(cpu->GetX());
  return CalculateCycles(code, false);
}

Byte PLA_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::PLA) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->SetA(cpu->StackPop());
  auto a = cpu->GetA();
  if (a == 0) {
    cpu->SetZeroFlag();
  } else if (IsNegative(a)) {
    cpu->SetNegativeFlag();
  }
  return CalculateCycles(code, false);
}

Byte PHA_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::PHA) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->StackPush(cpu->GetA());
  return CalculateCycles(code, false);
}

Byte PLP_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::PLP) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->SetStatus(cpu->StackPop());
  return CalculateCycles(code, false);
}

Byte PHP_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::PHP) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->StackPush(cpu->GetStatus());
  return CalculateCycles(code, false);
}

Byte BPL_handler(Byte code, CPU *cpu, MainBus *bus);
Byte BMI_handler(Byte code, CPU *cpu, MainBus *bus);
Byte BVC_handler(Byte code, CPU *cpu, MainBus *bus);
Byte BVS_handler(Byte code, CPU *cpu, MainBus *bus);
Byte BCC_handler(Byte code, CPU *cpu, MainBus *bus);
Byte BCS_handler(Byte code, CPU *cpu, MainBus *bus);
Byte BNE_handler(Byte code, CPU *cpu, MainBus *bus);
Byte BEQ_handler(Byte code, CPU *cpu, MainBus *bus);
Byte BRK_handler(Byte code, CPU *cpu, MainBus *bus);
Byte RTI_handler(Byte code, CPU *cpu, MainBus *bus);
Byte JSR_handler(Byte code, CPU *cpu, MainBus *bus);
Byte RTS_handler(Byte code, CPU *cpu, MainBus *bus);
Byte JMP_handler(Byte code, CPU *cpu, MainBus *bus);
Byte BIT_handler(Byte code, CPU *cpu, MainBus *bus);

Byte CLC_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::CLC) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->ClearCarryFlag();
  return CalculateCycles(code, false);
}

Byte SEC_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::SEC) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->SetCarryFlag();
  return CalculateCycles(code, false);
}

Byte CLD_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::CLD) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->ClearDecimalMode();
  return CalculateCycles(code, false);
}

Byte SED_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::SED) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->SetDecimalMode();
  return CalculateCycles(code, false);
}

Byte CLI_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::CLI) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->ClearInterruptDisable();
  return CalculateCycles(code, false);
}

Byte SEI_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::SEI) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->SetInterruptDisable();
  return CalculateCycles(code, false);
}

Byte CLV_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::CLV) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  cpu->ClearOverflowFlag();
  return CalculateCycles(code, false);
}

Byte NOP_handler(Byte code, CPU *cpu, MainBus *bus) {
  CHECK(opcode_matrix[code] == Opcode::NOP) << "Invalid code: " << code;
  cpu->IncreaseProgramCounter(1);
  return CalculateCycles(code, false);
}

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