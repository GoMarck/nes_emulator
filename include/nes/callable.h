#pragma once

#include <functional>

#include "nes/type.h"

namespace nes {

using WriteCallback = std::function<void(Byte)>;
using ReadCallbak = std::function<Byte(void)>;

class Callable {
 public:
  Callable() = default;
  virtual ~Callable() = default;
};

class WriteCallable : public Callable {
 public:
  WriteCallable() = default;

  ~WriteCallable() override = default;

  virtual void SetWriteCallback(Address address, std::function<void(Byte)>) = 0;
};

class ReadCallable : public Callable {
 public:
  ReadCallable() = default;

  ~ReadCallable() override = default;

  virtual void SetReadCallback(Address address, std::function<Byte(void)>) = 0;
};
}  // namespace nes