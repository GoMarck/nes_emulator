#include "nes/ppu.h"

namespace nes {

void PPU::PowerUp() {
  ppu_ctrl_ = 0;
  ppu_mask_ = 0;
  ppu_status_ = 0xa0;
  oam_addr_ = 0;
  ppu_scroll_ = 0;
  ppu_addr_ = 0;
  ppu_data_ = 0;
  w = false;
  odd_frame_ = false;
}

void PPU::Reset() {
  ppu_ctrl_ = 0;
  ppu_mask_ = 0;
  ppu_scroll_ = 0;
  ppu_data_ = 0;
  w = false;
  odd_frame_ = false;
}

};  // namespace nes