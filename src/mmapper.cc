#include "nes/cartridge.h"

#include "nes/macros.h"
#include "nes/mmaper.h"

namespace nes {

Byte NoMapper::ReadProgramData(Address address) {
  return cartridge_->ReadProgramData(address - 0x8000);
}

Byte NoMapper::ReadCharacterData(Address address) {
  return cartridge_->ReadCharacterData(address);
}

}  // namespace nes