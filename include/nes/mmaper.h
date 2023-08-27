#pragma once

#include <memory>
#include <utility>

#include "nes/cartridge.h"
#include "nes/logging.h"
#include "nes/type.h"

namespace nes {

// 用于扩展内存的一个装置，位于卡带中，不同的卡带携带有不同的mmaper
class MMapper {
 public:
  /// Construct from Cartridge.
  ///
  /// \param cartridge shared poniter of Cartridge instance.
  explicit MMapper(std::shared_ptr<Cartridge> cartridge)
      : cartridge_(std::move(cartridge)) {}

  virtual ~MMapper() = default;

  /// Read the PRG-ROM data via address.
  ///
  /// \param address PRG-ROM address.
  /// \return Byte read from PRG-ROM via address.
  virtual Byte ReadProgramData(Address address) = 0;

  /// Read the CHR-ROM data via address.
  ///
  /// \param address CHR-ROM address.
  /// \return Byte read from CHR-ROM via address.
  virtual Byte ReadCharacterData(Address address) = 0;

 protected:
  /// Cartridge instance for reading/writing ROM data.
  std::shared_ptr<Cartridge> cartridge_;

  /// Memory-mapper type.
  MMapperType type_;
};

/// On reset, the first PRG-ROM bank is loaded into $8000 and the last PRG-ROM
/// bank is loaded into $C000. Switching is only allowe for the bank at $8000,
/// the one at $C000 is permanently assigned to that location. Since this mapper
/// has no support for VROM, games using it have 8 KB of VRAM at $0000 in PPU
/// memory.
class NoMapper : public MMapper {
 public:
  explicit NoMapper(std::shared_ptr<Cartridge> cartridge)
      : MMapper(std::move(cartridge)) {
    type_ = MMapperType::NROM;
  }

  ~NoMapper() override = default;

  /// Read the PRG-ROM data via address.
  ///
  /// \param address PRG-ROM address.
  /// \return Byte read from PRG-ROM via address.
  Byte ReadProgramData(Address address) override;

  /// Read the CHR-ROM data via address.
  ///
  /// \param address CHR-ROM address.
  /// \return Byte read from CHR-ROM via address.
  Byte ReadCharacterData(Address address) override;
};

}  // namespace nes