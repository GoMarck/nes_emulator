#include "nes/cartridge.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "nes/logging.h"
#include "nes/marco.h"

namespace nes {

/// Header size is 16 bytes.
constexpr size_t HEADER_SIZE = 2 << 4;
/// Trainer size is 512 bytes.
constexpr size_t TRAINER_SIZE = 2 << 9;
/// PRG-ROM size is 16 KB.
constexpr size_t PRG_ROM_SIZE = 2 << 13;
/// CHR-ROM size is 8 KB.
constexpr size_t CHR_ROM_SIZE = 2 << 12;

Cartridge::Cartridge(std::string path) : path_(std::move(path)) {}

bool Cartridge::Init() {
  char *absoulte_path = static_cast<char *>(malloc(PATH_MAX + 1));
  RETURN_FALSE_REPENT_IF(absoulte_path == nullptr, "malloc failed!");
  auto raii = std::unique_ptr<char[]>(absoulte_path);
  RETURN_FALSE_REPENT_IF(
      realpath(path_.c_str(), absoulte_path) == nullptr,
      "parse file real path failed: " + std::string(strerror(errno)));
  path_ = std::string(absoulte_path);

  FILE *file = fopen(path_.c_str(), "rb");
  RETURN_FALSE_REPENT_IF(
      file == nullptr, "Failed to open file: " + std::string(strerror(errno)));

  fseek(file, 0L, SEEK_END);
  auto size = ftell(file);
  fseek(file, 0L, SEEK_SET);
  if (size <= HEADER_SIZE) {
    NES_LOG(ERROR) << "Invalid file format!";
    fclose(file);
    return false;
  }

  std::vector<Byte> contents;
  contents.reserve(size);
  if (!ReadRomFile(file, contents.data(), size)) {
    NES_LOG(ERROR) << "Read rom file failed!";
    fclose(file);
    return false;
  }
  fclose(file);

  RETURN_FALSE_IF(!ParseHeader(contents.data()));

  size_t curr_size = HEADER_SIZE;
  Byte *curr_data = contents.data() + HEADER_SIZE;
  if (exist_trainer_) {
    RETURN_FALSE_REPENT_IF(curr_size + TRAINER_SIZE <= size, "Not enough size for trainer");
    trainer_.resize(TRAINER_SIZE, 0);
    memcpy(trainer_.data(), curr_data, TRAINER_SIZE);
    curr_data += TRAINER_SIZE;
    curr_size += TRAINER_SIZE;
  }

  for (uint8_t i = 0; i < program_rom_bank_number_; ++i) {
    RETURN_FALSE_REPENT_IF(curr_size + PRG_ROM_SIZE <= size, "Not enough size for PRG-ROM");
    std::vector<Byte> program_rom(PRG_ROM_SIZE, 0);
    memcpy(program_rom.data(), curr_data, PRG_ROM_SIZE);
    program_data_.emplace_back(std::move(program_rom));
    curr_data += PRG_ROM_SIZE;
    curr_size += PRG_ROM_SIZE;
  }

  for (uint8_t i = 0; i < character_rom_bank_number_; ++i) {
    
    std::vector<Byte> character_rom(CHR_ROM_SIZE, 0);
    memcpy(character_rom.data(), curr_data, CHR_ROM_SIZE);
    character_data_.emplace_back(std::move(character_rom));
    curr_data += CHR_ROM_SIZE;
  }
  return true;
}

Byte Cartridge::Read(Address address) { return 0; }

bool Cartridge::ParseHeader(Byte *header) {
  RETURN_FALSE_REPENT_IF(header == nullptr, "header pointer is null");
  std::string identify(reinterpret_cast<char *>(header), 3);
  RETURN_FALSE_REPENT_IF(identify != "NES", "Invalid identifier: " + identify);
  Byte num = *(header + 3);
  RETURN_FALSE_REPENT_IF(num != 0x1A,
                         "Invalid identifier: " + std::to_string(num));

  program_rom_bank_number_ = *(header + 4);
  character_rom_bank_number_ = *(header + 5);

  Byte sixth_byte = *(header + 6);
  Byte seventh_byte = *(header + 7);
  Byte eighth_byte = *(header + 8);
  mirroring_type_ =
      (sixth_byte & 0x1) == 0 ? MirrorType::HORIZONTAL : MirrorType::VERTICAL;
  exist_battery_ram_ = (sixth_byte & 0x2) != 0;
  exist_trainer_ = (sixth_byte & 0x4) != 0;
  mmaper_number_ = (sixth_byte & 0xF) | (seventh_byte & 0xF0);
  ram_bank_number_ = eighth_byte == 0 ? 1 : eighth_byte;
  return true;
}

bool Cartridge::ReadRomFile(FILE *fp, Byte *data, size_t size) {
  size_t r;
  STREAM_RETRY_ON_EINTR(r, fp, fread(data, 1, size, fp));
  return r == size;
}

};  // namespace nes
