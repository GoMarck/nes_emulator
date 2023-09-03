#include "nes/cpu.h"

namespace nes {

void CPU::SetCarryFlag() {
  status_ |= 0x1;
}

void CPU::ClearCarryFlag() {
  status_ &= 0xFE;
}

void CPU::SetDecimalMode() {
  status_ |= 0x8;
}

void CPU::ClearDecimalMode() {
  status_ &= 0xF7; 
}

void CPU::SetInterruptDisable() {
  status_ |= 0x4;
}

void CPU::ClearInterruptDisable() {
  status_ &= 0xFB;
}

void CPU::ClearOverflowFlag() {
  status_ &= 0xBF;
}

bool CPU::Interruptible() const {
  return (status_ & 0x4) == 0;
}

};  // namespace nes