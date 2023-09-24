#include "nes/cpu.h"

#include <memory>

#include "nes/instruction.h"
#include "nes/logging.h"
#include "nes/main_bus.h"

namespace nes {

CPU::CPU(std::shared_ptr<MainBus> bus) : main_bus_(std::move(bus)) {
  PowerUp();
}

void CPU::PowerUp() {
  accumulator_ = 0;
  x_ = 0;
  y_ = 0;
  stack_pointer_ = 0xFD;
  status_ = 0x34;  // IRQ disabled.
}

void CPU::Reset() {
  stack_pointer_ -= 0x3;
  SetInterruptDisable();
}

void CPU::Tick() {}

void CPU::Execute() {
  if (skip_cycles_ != 0) {
    --skip_cycles_;
    return;
  }
  Byte opcode = main_bus_->Read(program_counter_);
  auto it = opcode_handler_map_.find(static_cast<Opcode>(opcode));
  if (it == opcode_handler_map_.end()) {
    NES_LOG(FATAL) << "Opcode handler undefined: " << opcode;
  }
  skip_cycles_ = it->second(opcode, this, main_bus_.get());
}

void CPU::SetCarryFlag() { status_ |= 0x1; }

void CPU::ClearCarryFlag() { status_ &= 0xFE; }

bool CPU::CarryFlag() const { return (status_ & 0x1) != 0; }

void CPU::SetZeroFlag() { status_ |= 0x2; }

void CPU::ClearZeroFlag() { status_ &= 0xFD; }

void CPU::SetDecimalMode() { status_ |= 0x8; }

void CPU::ClearDecimalMode() { status_ &= 0xF7; }

void CPU::SetInterruptDisable() { status_ |= 0x4; }

void CPU::ClearInterruptDisable() { status_ &= 0xFB; }

void CPU::SetOverflowFlag() { status_ |= 0x40; }

void CPU::ClearOverflowFlag() { status_ &= 0xBF; }

void CPU::SetNegativeFlag() { status_ |= 0x80; }

void CPU::ClearNegativeFlag() { status_ &= 0x7F; }

bool CPU::Interruptible() const { return (status_ & 0x4) == 0; }

};  // namespace nes