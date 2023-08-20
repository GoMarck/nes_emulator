# pragma once

#include <cstdint>

using Byte = uint8_t;
using Word = uint16_t;
using Address = uint16_t;

enum MMapperType : uint8_t {
  NROM = 0,
  MMC1 = 1,
  UNROM = 2,
  CNROM = 3,
  MMC3 = 4,
  MMC5 = 5
};