#pragma once

#include <memory>

#include "nes/callable.h"
#include "nes/main_bus.h"
#include "nes/type.h"

namespace nes {
class CPU : public WriteCallable, ReadCallable {
 public:
  CPU() = default;

  ~CPU() override = default;

 private:
};
}  // namespace nes