#pragma once

#include <memory>
#include <vector>

#include "nes/callable.h"
#include "nes/main_bus.h"
#include "nes/type.h"

namespace nes {

/// Central Processing Unit base on 6502.
class CPU : public WriteCallable, ReadCallable {
 public:
  CPU() = default;

  ~CPU() override = default;

  /// Reset the CPU to initial state.
  void Reset();

  /// Tick once, run a CPU clock cycle.
  void Tick();

  void SetWriteCallback(Address address,
                        std::function<void(Byte)> callback) override;

  void SetReadCallback(Address address,
                       std::function<Byte(void)> callback) override;

 private:
  /// Set status register interrupt flag.
  ///
  /// \param flag True indicates set interrupt disable flag.
  void SetInterruptDisable(bool flag);

  /// Get status register interrupt flag.
  ///
  /// \return True if interrupt disable flag is not set.
  bool Interruptible() const;

  /// The program counter is a 16-bit register which holds the address of the
  /// next instruction to be executed. As instructions are executed, the value
  /// of the program counter is updated, usually moving on to the next
  /// instruction in the sequence. The value can be affected by branch and jump
  /// instructions, procedure calls and interrupts.
  Word program_counter_;

  /// The stack is located at memory locations $0100-$01FF. The stack pointer is
  /// an 8-bit register which serves as an offset from $0100. The stack works
  /// top-down, so when a byte is pushed on to the stack, the stack pointer is
  /// decremented and when a byte is pulled from the stack, the stack pointer is
  /// incremented. There is no detection of stack overflow and the stack pointer
  /// will just wrap around from $00 to $FF.
  Byte stack_pointer_;

  /// The accumulator is an 8-bit register which stores the results of
  /// arithmetic and logic operations. The accumulator can also be set to a
  /// value retrieved from memory.
  Byte accumulator_;

  /// The X register is an 8-bit register typically used as a counter or an
  /// offset for certain addressing modes. The X register can be set to a value
  /// retrieved from memory and can be used to get or set the value of the stack
  /// pointer.
  Byte x_;

  /// The Y register is an 8-bit register which is used in the same way as the X
  /// register, as a counter or to store an offset. Unlike the X register, the Y
  /// register cannot affect the stack pointer.
  Byte y_;

  /// The status register contains a number of single bit flags which are set or
  /// cleared when instructions are executed.
  /// 
  /// 7  bit  0
  /// ---- ----
  /// NV#B DIZC
  /// |||| ||||
  /// |||| |||+- Carry Flag (1 on unsigned overflow)
  /// |||| ||+-- Zero Flag (1 when all bits of a result are 0)
  /// |||| |+--- Interrupt Disable (when 1, no interupts will occur 
  /// |||| |     (exceptions are IRQs forced by BRK and NMIs))
  /// |||| +---- Decimal Mode (1 when CPU in BCD mode)
  /// |||+------ Break Flag (1 when interupt was caused by a BRK)
  /// ||+------- Unused, always 1
  /// |+-------- Overflow Flag (1 on signed overflow)
  /// +--------- Negative Flag (1 when result is negative)
  Byte status_;

  std::vector<WriteCallback> write_callbacks_;

  std::vector<ReadCallbak> read_callbacks_;

  /// Main bus for memory access.
  std::shared_ptr<MainBus> main_bus_;
};

}  // namespace nes