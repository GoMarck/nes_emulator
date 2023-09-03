#include "nes/cpu.h"

#include "nes/instruction.h"
#include "nes/logging.h"

namespace nes {

void CPU::Reset() {}

void CPU::Tick() {}

void CPU::Execute() {
  Byte opcode = main_bus_->Read(program_counter_++);
  auto it = opcode_handler_list_.find(static_cast<Opcode>(opcode));
  if (it == opcode_handler_list_.end()) {
    NES_LOG(FATAL) << "Opcode handler undefined: " << opcode;
  }

  it->second(this, opcode_addr_mode_matrix[opcode]);
}

void CPU::SetCarryFlag() { status_ |= 0x1; }

void CPU::ClearCarryFlag() { status_ &= 0xFE; }

void CPU::SetDecimalMode() { status_ |= 0x8; }

void CPU::ClearDecimalMode() { status_ &= 0xF7; }

void CPU::SetInterruptDisable() { status_ |= 0x4; }

void CPU::ClearInterruptDisable() { status_ &= 0xFB; }

void CPU::ClearOverflowFlag() { status_ &= 0xBF; }

bool CPU::Interruptible() const { return (status_ & 0x4) == 0; }

};  // namespace nes