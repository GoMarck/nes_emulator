#pragma once

#include <functional>

#include "nes/type.h"

namespace nes {
class Callable {
 public:
  Callable() = default;
  virtual ~Callable() = default;
};

class WriteCallable : public Callable {
 public:
  WriteCallable() = default;

  ~WriteCallable() override = default;

  void SetWriteCallback(Address address, std::function<void(Byte)>);
};

class ReadCallable : public Callable {
 public:
  ReadCallable() = default;

  ~ReadCallable() override = default;

  void SetReadCallback(Address address, std::function<Byte(void)>);
};
}  // namespace nes