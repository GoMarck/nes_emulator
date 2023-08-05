#pragma once

#include <memory>
#include <vector>

#include "nes/picture_bus.h"
#include "nes/screen.h"
#include "nes/type.h"

namespace nes {
class PPU {
public:
  PPU(std::shared_ptr<PictureBus> bus, std::shared_ptr<IScreen> screen);
  ~PPU() noexcept;

  void Tick();
  
private:
  enum State {
    PRE_RENDER = 0,
    RENDER,
    POST_RENDER,
    VERTICAL_BLANK
  };

  // PPU access memory via Picture Bus. 
  std::shared_ptr<PictureBus> picture_bus_;

  // Screen for receiving the frame data.
  std::shared_ptr<IScreen> screen_;

  // Current VRAM address, only have 15 valid bits actually.
  Word v;

  // Tempoary VRAM address, only have 15 valid bits actually.
  Word t;

  // Fine X position.
  Byte x;

  // First/second write toggle. 
  bool w;

  // OAM.
  std::vector<Byte> oam_;

  // Secondary OAM
  std::vector<Byte> secondary_oam_;

  // Current Scanline number.
  int scanline_number_;

  // Current state.
  State state_;
};
}  // namespace nes