#pragma once

#include <string>
#include <vector>

#include "nes/type.h"

namespace nes {

/// Cartridge for reading the game‘s ROM data.
class Cartridge {
 public:
  /// Construct from game's rom path.
  ///
  /// \param path Relative or absolute path of game's rom file.
  explicit Cartridge(std::string path);

  ~Cartridge() = default;

  /// Init catridge, read all contents of the file and init the cartridge.
  ///
  /// \return True if initialize success.
  bool Init();

  /// Read the PRG-ROM data via address.
  ///
  /// \param address PRG-ROM address.
  /// \return Byte read from PRG-ROM via address.
  Byte ReadProgramData(Address address);

  /// Read the CHR-ROM data via address.
  ///
  /// \param address CHR-ROM address.
  /// \return Byte read from CHR-ROM via address.
  Byte ReadCharacterData(Address address);

  /// Get memory mapper type.
  ///
  /// \return MMaper type.
  MMapperType GetMMapperType() const {
    return static_cast<MMapperType>(mmaper_number_); 
  }

 private:
  friend class CartridgeTest;

  enum MirrorType { HORIZONTAL = 0, VERTICAL };

  /// Parse and verify the header.
  ///
  /// \param header Header content pointer.
  /// \return True if the file header is valid.
  bool ParseHeader(Byte *header);

  /// Read the whole ROM file.
  ///
  /// \param fp File pointer.
  /// \param data Memory address for storing the file data.
  /// \param size File size or expected read size, if the actual read data size
  /// is less then the size, error occured. \return True if read success.
  bool ReadRomFile(FILE *fp, Byte *data, size_t size);

  /// Game's rom file path.
  std::string path_;

  /// Numbers of 16KB PRG-ROM banks.
  Byte program_rom_bank_number_;

  /// Numbers of 8KB CHR-ROM/VROM banks.
  Byte character_rom_bank_number_;

  /// Number of 8 KB RAM banks.
  Byte ram_bank_number_;

  /// Mirroring type.
  MirrorType mirroring_type_;

  /// Indicate the presence of battery-backed RAM at memory locations
  /// $6000-$7FFF.
  bool exist_battery_ram_;

  /// Indicates the presence of a 512-byte trainer at memory locations
  /// $7000-$71FF.
  bool exist_trainer_;

  /// Mapper number.
  Byte mmaper_number_;

  /// Trainer data.
  std::vector<Byte> trainer_;

  /// PRG-ROM data.
  std::vector<Byte> program_data_;

  /// CHR-ROM data.
  std::vector<Byte> character_data_;
};

};  // namespace nes