#pragma once

#include <memory>
#include <utility>

#include "nes/cartridge.h"
#include "nes/logging.h"
#include "nes/type.h"

namespace nes {

/// Memory-mapper for CPU/PPU read/write data from/to ROM.
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
  virtual Byte ReadCharacterData(Address address);

 protected:
  /// Cartridge instance for reading/writing ROM data.
  std::shared_ptr<Cartridge> cartridge_;

  /// Memory-mapper type.
  MMapperType type_;
};

/// Reference from: https://www.nesdev.org/wiki/Programming_NROM
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
};

}  // namespace nes