#include "nes/cartridge.h"

#include <_stdio.h>
#include <malloc/_malloc.h>
#include <sys/syslimits.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "nes/logging.h"
#include "nes/marco.h"

namespace nes {

/// PRG-ROM size is 16 KB.
constexpr int PRG_ROM_SIZE = 2 << 13;
/// CHR-ROM size is 8 KB.
constexpr int CHR_ROM_SIZE = 2 << 12;

Cartridge::Cartridge(std::string path) : path_(std::move(path)) {}

bool Cartridge::Init() {
  char *absoulte_path = static_cast<char *>(malloc(PATH_MAX + 1));
  RETURN_FALSE_REPENT_IF(absoulte_path, "malloc failed!");
  auto raii = std::unique_ptr<char[]>(absoulte_path);
  RETURN_FALSE_REPENT_IF(
      realpath(path_.c_str(), absoulte_path) == nullptr,
      "parse file real path failed: " + std::string(strerror(errno)));
  path_ = std::string(absoulte_path);

  FILE *file = fopen(path_.c_str(), "r");  
  RETURN_FALSE_REPENT_IF(file == nullptr, "Failed to open file: " + std::string(strerror(errno))); 

  fseek(file, 0L, SEEK_END);
  auto file_size = ftell(file);
  fseek(file, 0L, SEEK_SET);  
  if (file_size == 0) {
    NES_LOG(ERROR) << "Empty file!";
    fclose(file);
    return false;
  }

  std::vector<Byte> contents = ReadRomFile(file, file_size);
  fclose(file);

  RETURN_FALSE_IF(ParseHeader(contents.data()));
  return true;
}

Byte Cartridge::Read(Address address) {}

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
  mmaper_number_ = (sixth_byte & 0x7) | (seventh_byte & 0x70);
  ram_bank_number_ = eighth_byte == 0 ? 1 : eighth_byte;
  return true;
}

};  // namespace nes
