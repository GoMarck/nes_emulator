#pragma once

#include <string>

#include "nes/type.h"

namespace nes {
class Cartridge {
 public:
  explicit Cartridge(std::string path);

  Byte Read(Address address);
};
};  // namespace nes